#include <hollowpch.h>
#include "PhysicsSystem.h"
#include "Hollow/Components/Transform.h"
#include "Hollow/Components/Body.h"
#include "Hollow/Components/Collider.h"
#include "Hollow/Managers/FrameRateController.h"
#include "Hollow/Managers/InputManager.h"
#include "Hollow/Physics/Broadphase/DynamicAABBTree.h"
#include "Hollow/Physics/NarrowPhase/SAT.h"
#include "Hollow/Managers/PhysicsManager.h"
#include "Hollow/Physics/Broadphase/Shape.h"
#include "Hollow/Managers/ImGuiManager.h"
#include "Hollow/Managers/EventManager.h"

namespace Hollow
{
	PhysicsSystem PhysicsSystem::instance;


	void PhysicsSystem::AddGameObject(GameObject* object)
	{

		if (CheckAllComponents<Collider>(object)) {
			// Collider Init
			Collider* pCol = static_cast<Collider*>(object->GetComponent<Collider>());

			pCol->mpTr = static_cast<Transform*>(object->GetComponent<Transform>());
			pCol->mpBody = static_cast<Body*>(object->GetComponent<Body>());

			if (!pCol->mIsTrigger)
			{
				glm::mat3 inertia = glm::mat3(0.0f);
				if (pCol->mpShape->mType == ShapeType::BOX) {
					inertia[0][0] = pCol->mpBody->mMass / 12.0f * (pCol->mpTr->mScale.y * pCol->mpTr->mScale.y + pCol->mpTr->mScale.z * pCol->mpTr->mScale.z);
					inertia[1][1] = pCol->mpBody->mMass / 12.0f * (pCol->mpTr->mScale.x * pCol->mpTr->mScale.x + pCol->mpTr->mScale.z * pCol->mpTr->mScale.z);
					inertia[2][2] = pCol->mpBody->mMass / 12.0f * (pCol->mpTr->mScale.y * pCol->mpTr->mScale.y + pCol->mpTr->mScale.x * pCol->mpTr->mScale.x);
				}
				else if (pCol->mpShape->mType == ShapeType::BALL)
				{
					inertia[0][0] = (2.0f / 5.0f) * pCol->mpBody->mMass * (pCol->mpTr->mScale.x * pCol->mpTr->mScale.x);
					inertia[1][1] = (2.0f / 5.0f) * pCol->mpBody->mMass * (pCol->mpTr->mScale.x * pCol->mpTr->mScale.x);
					inertia[2][2] = (2.0f / 5.0f) * pCol->mpBody->mMass * (pCol->mpTr->mScale.x * pCol->mpTr->mScale.x);
				}

				if (pCol->mpBody->mInverseMass == 0.0f)
					pCol->mpBody->mLocalInertiaInverse = glm::mat3(0);
				else
					pCol->mpBody->mLocalInertiaInverse = glm::inverse(inertia);

				pCol->mpBody->mPosition = pCol->mpTr->mPosition;
				pCol->mpBody->mPreviousPosition = pCol->mpTr->mPosition;
				pCol->mpBody->mQuaternion = pCol->mpTr->mQuaternion;
				pCol->mpBody->mPreviousQuaternion = pCol->mpTr->mQuaternion;
				pCol->mpBody->mRotationMatrix = glm::toMat3(pCol->mpBody->mQuaternion);
			}

			PhysicsManager::Instance().UpdateScale(object);
			
			// Collider added to Dynamic BVH
			PhysicsManager::Instance().mTree.AddCollider(pCol);
		}
	}

	void PhysicsSystem::DebugContacts()
	{
		ImGui::Begin("Contacts");
		{
			for (auto points : *PhysicsManager::Instance().mSAT.mContacts)
			{
				for (int i = 0; i < points->contactPoints.size(); i++)
				{
					ImGui::Text("Point location- %f , %f , %f ", points->contactPoints[i].point.x, points->contactPoints[i].point.y, points->contactPoints[i].point.z);
					ImGui::Text("Penetration depth - %f", points->contactPoints[i].penetrationDepth);
				}
			}
		}
		ImGui::End();
	}

	void PhysicsSystem::OnDeleteGameObject(GameObject* pGameObject)
	{
		PhysicsManager::Instance().mTree.RemoveCollider(pGameObject->GetComponent<Collider>());
	}

	void PhysicsSystem::OnDeleteAllGameObjects()
	{
		PhysicsManager::Instance().mSAT.ResetContacts();
		PhysicsManager::Instance().mTree.DeleteTree();
	}

	void PhysicsSystem::UpdateTree()
	{
		for (unsigned int i = 0; i < mGameObjects.size(); ++i)
		{
			if(mGameObjects[i]->mType == 0)
			{
				mGameObjects[i]->GetComponent < Hollow::Body >()->mPosition += glm::vec3(0.0f, 0.0f, 0.0f);
			}
			
			PhysicsManager::Instance().UpdateScale(mGameObjects[i]);
		}

		// balancing the tree
		PhysicsManager::Instance().mTree.Update();
	}

	void PhysicsSystem::Step(float fixedDeltaTime)
	{
		UpdateTree();

		// finds out intersecting bounding boxes
		PhysicsManager::Instance().mTree.CalculatePairs();

		std::list < std::pair<Collider*, Collider*>>& pairs = PhysicsManager::Instance().mTree.GetPairs();

		SAT& mSAT = PhysicsManager::Instance().mSAT;

		for (auto& pair : pairs)
		{

			if (pair.first->mIsTrigger || pair.second->mIsTrigger)
			{
				int id1 = pair.first->mpOwner->mType;
				int id2 = pair.second->mpOwner->mType;

				EventManager::Instance().FireCollisionEvent(BIT(id1) | BIT(id2), pair.first->mpOwner, pair.second->mpOwner);
				
				continue;
			}
			// perform the SAT intersection test for cubes/cubes, spheres/cubes, and sphere/sphere
			mSAT.CheckCollsionAndGenerateContact(pair.first, pair.second);
		}

		for (auto go : mGameObjects)
		{
			Body* pBody = static_cast<Body*>(go->GetComponent<Body>());

			if (pBody != nullptr) {

				if (pBody->mBodyType == Body::STATIC)
					continue;
				// compute acceleration
				glm::vec3 acc = pBody->mTotalForce * pBody->mInverseMass;
				if (pBody->mUseGravity)
				{
					acc += gravity;
				}
				
				glm::vec3 alpha = pBody->mTotalTorque * pBody->mWorldInertiaInverse;

				// integrate acc into the velocity
				pBody->mVelocity += acc * fixedDeltaTime;
				pBody->mAngularVelocity += alpha * fixedDeltaTime;

				// set forces to zero
				pBody->mTotalForce = glm::vec3(0);
				pBody->mTotalTorque = glm::vec3(0);
			}
		}

		// TODO remove loops and replace with hashmap
		if (!mSAT.mPrevContacts->empty())
		{
			for (auto itOld : *mSAT.mPrevContacts)
			{
				for (auto itNew : *mSAT.mContacts)
				{
					if (((*itNew).bodyA == (*itOld).bodyA &&
						(*itNew).bodyB == (*itOld).bodyB)) {

						// iterate through contact points
						for (int j = 0; j < (*itOld).contactPoints.size(); ++j)
						{
							Contact& cOld = (*itOld).contactPoints[j];

							for (int k = 0; k < (*itNew).contactPoints.size(); ++k) {
								Contact& cNew = (*itNew).contactPoints[k];

								if (glm::distance2(cNew.point, cOld.point) < proximityEpsilon)
								{
									cNew.normalImpulseSum = cOld.normalImpulseSum * bias;
									cNew.tangentImpulseSum1 = cOld.tangentImpulseSum1 * bias;
									cNew.tangentImpulseSum2 = cOld.tangentImpulseSum2 * bias;

									// apply old impulse as warm start
									//(itNew)->constraint.EvaluateJacobian(&cNew, itNew->collisionNormal);
									itNew->constraint.ApplyImpulse(itNew->bodyA, itNew->bodyB, cNew.mMatxjN, cNew.normalImpulseSum);
									itNew->constraint.ApplyImpulse(itNew->bodyA, itNew->bodyB, cNew.mMatxjT0, cNew.tangentImpulseSum1);
									itNew->constraint.ApplyImpulse(itNew->bodyA, itNew->bodyB, cNew.mMatxjT1, cNew.tangentImpulseSum2);
								}
							}

						}

					}
				}
			}
		}

		/*DebugContacts();*/

		for (int i = 0; i < impulseIterations; ++i) {
			for (auto c : *mSAT.mContacts) {

				//std::cout << c->contactPoints[0]->normalImpulseSum << std::endl;
				int pointCount = static_cast<int>(c->contactPoints.size());

				for (int j = 0; j < pointCount; ++j) {

					c->constraint.EvaluateVelocityJacobian(c->bodyA, c->bodyB);

					//===== solve for normal constraint
					// bias value
					float b = baumgarte / fixedDeltaTime * std::min(c->contactPoints[j].penetrationDepth - slop, 0.0f);

					float lambda = -c->contactPoints[j].effectiveMassN *
						(c->constraint.JacobianJacobianMult(c->contactPoints[j].jacobianN, c->constraint.velocityJacobian) + b);

					float origNormalImpulseSum = c->contactPoints[j].normalImpulseSum;

					c->contactPoints[j].normalImpulseSum += lambda;
					c->contactPoints[j].normalImpulseSum =
						glm::clamp(c->contactPoints[j].normalImpulseSum, 0.0f, std::numeric_limits<float>::infinity());

					float deltaLambda = c->contactPoints[j].normalImpulseSum - origNormalImpulseSum;

					c->constraint.ApplyImpulse(c->bodyA, c->bodyB, c->contactPoints[j].mMatxjN, deltaLambda);

					if (!(c->bodyA->mIsFrictionLess || c->bodyB->mIsFrictionLess)) {
						c->constraint.EvaluateVelocityJacobian(c->bodyA, c->bodyB);

						//float nLambda = c->contactPoints[j]->normalImpulseSum;
						float nLambda = -gravity.y / pointCount;

						//==== solve for tangent 0
						lambda = -c->contactPoints[j].effectiveMassT0 *
							(c->constraint.JacobianJacobianMult(c->contactPoints[j].jacobianT0, c->constraint.velocityJacobian) + 0.0f);

						float origTangent0ImpulseSum = c->contactPoints[j].tangentImpulseSum1;

						c->contactPoints[j].tangentImpulseSum1 += lambda;
						c->contactPoints[j].tangentImpulseSum1 =
							glm::clamp(c->contactPoints[j].tangentImpulseSum1, -c->frictionConstant * nLambda, c->frictionConstant * nLambda);

						deltaLambda = c->contactPoints[j].tangentImpulseSum1 - origTangent0ImpulseSum;

						c->constraint.ApplyImpulse(c->bodyA, c->bodyB, c->contactPoints[j].mMatxjT0, deltaLambda);

						//==== solve for tangent 1
						c->constraint.EvaluateVelocityJacobian(c->bodyA, c->bodyB);

						lambda = -c->contactPoints[j].effectiveMassT1 *
							(c->constraint.JacobianJacobianMult(c->contactPoints[j].jacobianT1, c->constraint.velocityJacobian) + 0.0f);

						float origTangent1ImpulseSum = c->contactPoints[j].tangentImpulseSum2;

						c->contactPoints[j].tangentImpulseSum2 += lambda;
						c->contactPoints[j].tangentImpulseSum2 =
							glm::clamp(c->contactPoints[j].tangentImpulseSum2, -c->frictionConstant * nLambda, c->frictionConstant * nLambda);

						deltaLambda = c->contactPoints[j].tangentImpulseSum2 - origTangent1ImpulseSum;

						c->constraint.ApplyImpulse(c->bodyA, c->bodyB, c->contactPoints[j].mMatxjT1, deltaLambda);
					}
				}

			}
		}

		mSAT.CopyContacts();

		for (auto go : mGameObjects)
		{
			Body* pBody = static_cast<Body*>(go->GetComponent<Body>());
			if (pBody != nullptr) {
				// save current position
				pBody->mPreviousPosition = pBody->mPosition;

				// integrate the position
				pBody->mPosition += pBody->mVelocity * fixedDeltaTime;

				pBody->mPreviousQuaternion = pBody->mQuaternion;

				if(pBody->mIsAlwaysVertical)
				{
					pBody->mAngularVelocity.x = pBody->mAngularVelocity.z = 0.0f;
				}
				
				// integrate the orientation
				glm::fquat newQuat = 0.5f * (pBody->mAngularVelocity) * pBody->mQuaternion * fixedDeltaTime;
				pBody->mQuaternion *= newQuat;
			}
		}
	}

	void PhysicsSystem::InterpolateState(float blendingFactor)
	{
		// Create Collision Events (Not the best place but at least gets called once per frame for sure)
		for (auto& contact : *PhysicsManager::Instance().mSAT.mPrevContacts)
		{
			int id1 = contact->bodyA->mpOwner->mType;
			int id2 = contact->bodyB->mpOwner->mType;

			EventManager::Instance().FireCollisionEvent(BIT(id1) | BIT(id2), contact->bodyA->mpOwner, contact->bodyB->mpOwner);
		}
		
		for (auto go : mGameObjects)
		{
			Body* pBody = static_cast<Body*>(go->GetComponent<Body>());
			Transform* pTr = static_cast<Transform*>(go->GetComponent<Transform>());

			if (pBody != nullptr) {
				pTr->mPosition = glm::mix(pBody->mPreviousPosition, pBody->mPosition, blendingFactor);

				// if there is a significant change in position or orientation only then update transformation matrix
				if (glm::length2(pBody->mPreviousPosition - pBody->mPosition) > 0.0001f) {
					pTr->dirtyBit = true;
				}

				pBody->mQuaternion = glm::normalize(pBody->mQuaternion);
				pTr->mQuaternion = glm::slerp(pBody->mPreviousQuaternion, pBody->mQuaternion, blendingFactor);
				if (pBody->mPreviousQuaternion != pBody->mQuaternion)
					pTr->dirtyBit = true;

				pBody->mRotationMatrix = glm::toMat3(pBody->mQuaternion);
				pBody->mWorldInertiaInverse =
					pBody->mRotationMatrix *
					pBody->mLocalInertiaInverse *
					glm::transpose(pBody->mRotationMatrix);
				//pTr->mRotation = pBody->mRotationMatrix;
			}
		}
	}

	void PhysicsSystem::Update()
	{
		PhysicsManager& pm = PhysicsManager::Instance();
		InputManager& input = InputManager::Instance();
		
		pm.isPaused = input.IsKeyTriggered("P") == true ? !pm.isPaused : pm.isPaused;

		nextStep = input.IsKeyTriggered("SPACE");

		/*
		 *Raycast Result Imgui window
		ImGui::Begin("RayCast Result");
		ImGui::Text("Mouse X : %f", input.GetMouseX());
		ImGui::Text("Mouse Y : %f", input.GetMouseY());
		ImGui::End();
		*/
		
		if (input.IsMouseButtonTriggered(SDL_BUTTON_LEFT))
		{
			GameObject* pObj = pm.CastRay();

			if (pObj)
			{
				ImGuiManager::Instance().mpSelectedGameObject = pObj;
				ImGuiManager::Instance().mSelectedGameObjectID = pObj->mID;
			}
		}
		/*
		 *Tree Debug File generator
		if(input.IsKeyTriggered("t"))
		{
			FILE* file;
			fopen_s(&file,"C:\\Users\\spand\\Desktop\\tree.txt", "w+");
			pm.DebugTree(pm.mTree.GetRoot(), file);
			fclose(file);
		}
		*/
		//================Physics Update======================
		float dt = FrameRateController::Instance().GetFrameTime();


		if (!pm.isPaused) {
			accumulator += dt;
			while (accumulator > maxPossibleDeltaTime) {
				//{
					//Timer t;
				Step(maxPossibleDeltaTime);
				//}
				accumulator -= maxPossibleDeltaTime;
			}
			InterpolateState(accumulator / maxPossibleDeltaTime);
		}
		else if (nextStep) {
			accumulator = 0.0f;
			Step(maxPossibleDeltaTime);
			InterpolateState(1.0f);
		}

	}
}
