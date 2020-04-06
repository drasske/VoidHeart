-- Put your cool cheat codes here!

function MuteAudio()
	if IsKeyTriggered("M") then
		gameMuted = not gameMuted
		Mute(gameMuted)
	end
end

function SkipToNextFloor()
	if IsKeyTriggered("F") then
		MoveToNextFloor()
	end
end

function KillAllEnemiesInRoom()
	if IsKeyTriggered("K") then
		-- Get the current room/enemies
		local floor = GetDungeonFloor(currentFloor)
		local currentRoomObj = floor:GetRoomFromIndex(currentRoom)
		local enemies = currentRoomObj.Enemies
	
		-- Set all enemies hp to -1
		for k=1,#enemies do
			enemies[k]:GetHealth().hitPoints = -1
		end
	end
end

function SkipToBoss()
	if IsKeyTriggered("B") then
		-- Put player in boss room
		local floor = GetDungeonFloor(currentFloor)
		local coords = floor:GetRoomFromIndex(floor:GetBossIndex()):GetCoords()
		currentRoom = coords.x * dungeonLength + coords.y
		player:GetBody().position = vec3.new(coords.y * roomSize + 2, 1.5, coords.x * roomSize + 2)
		player:GetTransform().position = player:GetBody().position
	end
end

function KillPlayer()
	if IsKeyTriggered("L") then
		player:GetHealth().hitPoints = -1
	end
end

function CheckCheatCodes()
	MuteAudio()
	SkipToNextFloor()
	KillAllEnemiesInRoom()
	SkipToBoss()
	KillPlayer()
end

CheckCheatCodes()