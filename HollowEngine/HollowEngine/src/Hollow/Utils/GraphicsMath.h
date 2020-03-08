#pragma once
#include "Hollow/Core/Core.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "Hollow/Utils/Log.h"

namespace Hollow
{
	/// <summary>
	/// Class GraphicsMath.
	/// </summary>
	class GraphicsMath
	{
	public:
		/// <summary>
		/// Generates Rotation matrix from direction vector.
		/// </summary>
		/// <param name="direction">The direction vector.</param>
		/// <returns>glm.mat4.</returns>
		HOLLOW_API static glm::mat4 RotationFromDirection(const glm::vec3& direction)
		{
			float angle = std::atan2(direction.y, direction.x);
			glm::mat4 glmrotXY = glm::rotate(angle, glm::vec3(0.0f, 0.0f, 1.0f));
			// Find the angle with the xy with plane (0, 0, 1); the - there is because we want to 
			// 'compensate' for that angle (a 'counter-angle')
			float angleZ = -std::asin(direction.z);
			// Make the matrix for that, assuming that Y is your 'side' vector; makes the model 'pitch'
			glm::mat4 glmrotZ = glm::rotate(angleZ, glm::vec3(0.0f, 1.0f, 0.0f));
			return glmrotXY * glmrotZ;
		}

		HOLLOW_API static glm::mat4 RotationFromTwoVectors(const glm::vec3& vector1, const glm::vec3& vector2)
		{
			glm::vec3 v = glm::cross(vector1, vector2);
			float s = glm::length(v);
			float c = glm::dot(vector1,vector2);
			if(c == -1.0f)
			{
				HW_CORE_ERROR("Opposite Facing vectors");
			}

			glm::mat4 I(1.0f);
			glm::mat4 Vx = glm::matrixCross4(v);

			return I + Vx + Vx * Vx * (1 / (1 + c));
		}

		/// <summary>
		/// Gets the Translation vector from Model matrix.
		/// </summary>
		/// <param name="matrix">The matrix.</param>
		/// <returns>glm.vec3.</returns>
		HOLLOW_API static glm::vec3 TranslateFromMatrix(const glm::mat4& matrix)
		{
			return xyz(matrix[3]);
		}

		/// <summary>
		/// Gets the Scale vector from Model matrix.
		/// </summary>
		/// <param name="matrix">The matrix.</param>
		/// <returns>glm.vec3.</returns>
		HOLLOW_API static glm::vec3 ScaleFromMatrix(const glm::mat4& matrix)
		{
			glm::vec3 a = xyz(matrix[0]);
			glm::vec3 b = xyz(matrix[1]);
			glm::vec3 c = xyz(matrix[2]);

			return glm::vec3(glm::length(a), glm::length(b), glm::length(c));
		}

		/// <summary>
		/// Gets the quaternion of Rotations from Model matrix.
		/// </summary>
		/// <param name="matrix">The matrix.</param>
		/// <returns>glm.quat.</returns>
		HOLLOW_API static glm::quat RotationFromMatrix(glm::mat4 matrix)
		{
			matrix[3].x = 0.0f; matrix[3].y = 0.0f; matrix[3].z = 0.0f;

			glm::vec3 a = xyz(matrix[0]);
			glm::vec3 b = xyz(matrix[1]);
			glm::vec3 c = xyz(matrix[2]);

			glm::vec3 s = glm::vec3(glm::length(a), glm::length(b), glm::length(c));

			matrix[0] /= s.x;
			matrix[1] /= s.y;
			matrix[2] /= s.z;

			return glm::toQuat(matrix);
		}

		/// <summary>
		/// Get the Scale vector and the quaternion of rotation from the Model matrix.
		/// </summary>
		/// <param name="matrix">The matrix.</param>
		/// <returns>std.pair&lt;_Ty1, _Ty2&gt;.</returns>
		HOLLOW_API static std::pair<glm::vec3, glm::quat> ScaleRotationFromMatrix(glm::mat4 matrix)
		{
			matrix[3].x = 0.0f; matrix[3].y = 0.0f; matrix[3].z = 0.0f;

			glm::vec3 a = xyz(matrix[0]);
			glm::vec3 b = xyz(matrix[1]);
			glm::vec3 c = xyz(matrix[2]);

			glm::vec3 s = glm::vec3(glm::length(a), glm::length(b), glm::length(c));

			matrix[0] /= s.x;
			matrix[1] /= s.y;
			matrix[2] /= s.z;

			return std::make_pair(s, glm::toQuat(matrix));
		}
	};
}