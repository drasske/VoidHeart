#pragma once

#include "Broadphase.h"
#include "Hollow/Components/Collider.h"
#include "Shape.h"
#include "Hollow/Utils/UniqueID.h"

namespace Hollow {
	class Node {
	public:
		Node() {
			ID = Hollow::GenerateUniqueID<Node>();
			
			aabb = new Hollow::ShapeAABB(glm::vec3(0), glm::vec3(0));
			left = nullptr;
			right = nullptr;
			parent = nullptr;

			height = 0;
			mClientData = nullptr;
		}

		ShapeAABB* aabb;
		Node* left;
		Node* right;
		Node* parent;

		unsigned int ID;
		
		int height;
		void* mClientData;

		Node* GetSibling() {
			return this == parent->left ? parent->right : parent->left;
		}

		bool IsLeaf(void) const
		{
			return left == nullptr;
		}

		void UpdateAABB(float fat) {
			if (IsLeaf()) {
				glm::vec3 fatMargin = glm::vec3(fat, fat, fat);
				aabb->fatten(static_cast<Collider*>(mClientData)->mpShape, fatMargin);
			}
			else {
				aabb->Merge(*left->aabb, *right->aabb);
			}
		}
	};

	class DynamicAABBTree : public Broadphase
	{
	public:
		DynamicAABBTree();
		~DynamicAABBTree();

		void RemoveCollider(Collider* col);

		void AddCollider(Collider* col);

		void Update();

		//virtual const std::list<ColliderPair>& CalculatePairs() = 0;
		void CalculatePairs();

		void DeleteTree();

		void SyncHierarchy(Node* parent);

		void CollisionQuery(const Shape& shape, std::vector<Collider*>& colliders);

		Node* GetRoot() {
			return root;
		}

		std::list<ColliderPair>& GetPairs() {
			return colliderPairs;
		}

	private:
		void InsertNode(Node* node, Node** parent);
		Node** SelectNode(Node* insertingNode, Node** left, Node** right);
		Node* RemoveNode(Node* node);
		std::vector<Node*> unfitNodes;

		// ComputePair 
		std::list<ColliderPair> colliderPairs;
		void SelfQuery(Node* A, Node* B);
		void SplitNodes(Node*, Node*);
		void SelfQuery(Node*);

		Node* root;
		int nodecount;
		float fatteningMargin;
	};

}
