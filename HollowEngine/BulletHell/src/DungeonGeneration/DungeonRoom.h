#pragma once
#include "GameMetaData/DungeonRoomType.h"
#include <glm/glm.hpp>
#include "Hollow.h"
#include "Hollow/Core/GameObject.h"

namespace BulletHell
{
    class DungeonFloor;
    class DungeonManager;
    class DungeonRoom
    {
        friend DungeonFloor;
    public:
        enum DoorDirrection
        {
            NONE = 0,
            UP = 1,
            RIGHT = 2,
            DOWN = 4,
            LEFT = 8
        };

        DungeonRoom();
        DungeonRoom(DungeonRoomType roomType, int roomID, int doors
            , int floorNum, int xCoord, int yCoord);

        void Set(DungeonRoomType roomType, int roomID, int doors
            , int floorNum, int xCoord, int yCoord);
        DungeonRoom& operator=(const DungeonRoom& room);
        glm::ivec2 GetCoords() const;
		int GetFloorNum() const { return mFloorNum; }
		int GetDoorBits() const { return mDoors; }
        int GetEnemyCount() const { return mEnemies.size(); }
		glm::vec3 GetCenter() const {
			return glm::vec3(mRow * mRoomSize + mRoomSize / 2.0f,
				0.0f,
				mColumn * mRoomSize + mRoomSize / 2.0f);
		}
    private:
        int TotalDoors() const;
    public:
        void ConstructRoom();
		bool IsCleared();
		void UnlockRoom();
		void LockDownRoom();
        int GetID() const;
        ~DungeonRoom();

		// structure
        static float mWallLength;
        static float mWallHeight;
        static float mRoomSize;
        static float mWallThickness;
        static float mDoorWidth;
        static float mDoorHeight;
        static float mDoorThickness;

		// game
        bool mIsUnlocked;
        bool mIsLockedFirstTime;
        Hollow::GameObject* mDoorGOs[9]; // doors at 1,2,4,8 indices
        std::vector<Hollow::GameObject*> mEnemies;
		std::vector<Hollow::GameObject*> mObstacles;
        DungeonRoomType mRoomType;
    private:    	
        Hollow::GameObject* mMinimapObject;
        std::string mMinimapTexturePath;
        int mRoomID;   // unique id of the room
        int mDoors;    // flag field for 4 doors
        int mFloorNum; // floor number of the room (owner id)
        int mColumn;   // position in the grid of the floor
        int mRow;      // position in the grid of the floor
        int mDistFromEntrance;

		float mLightOffsetFromDoor;
    };
}
