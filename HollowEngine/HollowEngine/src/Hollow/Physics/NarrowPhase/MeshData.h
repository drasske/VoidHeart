#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <map>

// The Half Edge Mesh Structure
struct MeshData {

	struct Vertex {
		glm::vec3 point;
		int edge; // from edge
	};

	struct Edge {
		int toVertex; // as in from and to
		int face;
		int next;
		int prev;
		int twin;

		Edge() {
			face = next = prev = twin = toVertex = -1;
		}
	};

	struct Face {
		// one of the edges
		int edge;
		glm::vec3 normal;
	};

	void AddVertex(glm::vec3 vert) {
		Vertex ver;
		ver.point = vert;
		ver.edge = -1;

		vertices[vIndex++] = ver;
	}
	void AddFace(const std::vector<int>& fVerts) {
		Face f;
		f.edge = -1;
		int faceIndex = fIndex;
		faces[fIndex++] = f;


		int edge = -1;
		int edgeTwin = -1;

		// create edges
		// loops over 3-0, 0-1, 1-2, 2-3
		int faceEdgeIndices[4] = { -1,-1,-1,-1 };
		for (int i = 3, j = 0; j < 4; i = j++) {
			// check if edge was in the map already
			auto edgeIter = edgeMap.find(std::pair<int, int>(fVerts[i], fVerts[j]));
			if (edgeIter != edgeMap.end()) {
				edge = edgeIter->second;
				edgeTwin = edges[edge].twin;
			}
			else {
				// if edge was not in the map, then create the edges and set all values
				Edge e1, e2;
	
				edges[eIndex] = (e1);
				edges[eIndex +1] = (e2);

				// set edge indices
				edge = eIndex;
				edgeTwin = edge + 1;

				// set twins
				edges[edge].twin = edgeTwin;
				edges[edgeTwin].twin = edge;

				// add to edge map
				edgeMap[std::pair<int, int>(fVerts[i], fVerts[j])] = edge;
				edgeMap[std::pair<int, int>(fVerts[j], fVerts[i])] = edgeTwin;

				eIndex += 2;
			}

			// link vertices and edges and faces
			if (edges[edge].toVertex < 0) {
				edges[edge].toVertex = fVerts[j];
			}
			if (edges[edgeTwin].toVertex < 0) {
				edges[edgeTwin].toVertex = fVerts[i];
			}
			if (edges[edge].face < 0) {
				edges[edge].face = faceIndex;
			}
			if (vertices[fVerts[i]].edge < 0) {
				vertices[fVerts[i]].edge = edge;
			}
			if (faces[faceIndex].edge < 0) {
				faces[faceIndex].edge = edge;
			}

			faceEdgeIndices[i] = edge;
		}

		// loop over the faceEdgeIndices to set next and prev for all halfEdges
		for (int i = 3, j = 0; j < 4; i = j++) {
			edges[faceEdgeIndices[i]].next = faceEdgeIndices[j];
			edges[faceEdgeIndices[j]].prev = faceEdgeIndices[i];
		}

		// calculate normal 
		glm::vec3 edge1 = GetEdgeDirection(edge);
		glm::vec3 edge2 = GetEdgeDirection(edges[edge].next);

		faces[faceIndex].normal = glm::normalize(glm::cross(edge1, edge2));
	}

	// Helper Functions
	glm::vec3 GetEdgeDirection(int edgeIndex) {
		glm::vec3 start = vertices[edges[edges[edgeIndex].prev].toVertex].point;
		glm::vec3 end = vertices[edges[edgeIndex].toVertex].point;

		return end - start;
	}
	glm::vec3 GetPointOnFace(int faceIndex) {
		int faceEdge = faces[faceIndex].edge;

		return vertices[edges[faceEdge].toVertex].point;
	}
	glm::vec3 GetSupport(const glm::vec3& direction) {
		int maxIndex = 0;
		float maxProjection = -std::numeric_limits<float>::infinity();

		for (int i = 0; i < 8; ++i) {
			float projection = glm::dot(direction, vertices[i].point);
			if (projection > maxProjection) {
				maxIndex = i;
				maxProjection = projection;
			}
		}
		return vertices[maxIndex].point;
	}
	std::vector<glm::vec3> GetFacePolygon(int faceIndex) {
		std::vector<glm::vec3> faceVerts;
		int edgeIndex = faces[faceIndex].edge;

		faceVerts.push_back(vertices[edges[edgeIndex].toVertex].point);
		edgeIndex = edges[edgeIndex].next;

		faceVerts.push_back(vertices[edges[edgeIndex].toVertex].point);
		edgeIndex = edges[edgeIndex].next;

		faceVerts.push_back(vertices[edges[edgeIndex].toVertex].point);
		edgeIndex = edges[edgeIndex].next;

		faceVerts.push_back(vertices[edges[edgeIndex].toVertex].point);

		return faceVerts;
	}

	// Vectors to store vertices, edges and faces
	std::array<Vertex, 8> vertices;
	std::array<Edge, 24> edges;
	std::array<Face, 6> faces;

	int vIndex = 0;
	int eIndex = 0;
	int fIndex = 0;

	// Map to store the edges
	std::map<std::pair<int, int>, int> edgeMap;
};