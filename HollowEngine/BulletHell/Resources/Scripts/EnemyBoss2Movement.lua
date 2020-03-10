function RunAtThePlayer()
    -----------------------------------------
    -- playtesting vars
	local enemySpeed = 20.0
	local scareSpeed = 8.0
    local scareDistance = 10 -- distance at which enemy will run away
    -----------------------------------------
    local transform = gameObject:GetTransform()
	local position = transform.position
    --  acquire target 
    local target = player
    if (target == nil or target.isActive == false) then
        return
    end
    local targetTransform = target:GetTransform()
    local targetPos = targetTransform.position
    
    -- calculate direction
    local xDir = targetPos.x - position.x
	local zDir = targetPos.z - position.z
	local dirLength = math.sqrt(xDir*xDir + zDir*zDir)
	local xDirNorm = xDir / dirLength
	local zDirNorm = zDir / dirLength
	
    -- look at the target
    local rot = vec3.new(0.0, 0.0, 0.0)
    local tangent = xDirNorm / zDirNorm
    local radians = math.atan(tangent)
    local degree = radians * 180 / math.pi
    if zDirNorm >= 0 then  
	    rot = vec3.new(0.0, degree, 0.0)
        transform:Rotate(rot)
    end
    if zDirNorm < 0 then 
	    rot = vec3.new(0.0, degree + 180, 0.0)
        transform:Rotate(rot)
    end
    
    -- setting the velocity
    local body = gameObject:GetBody()
    body.velocity = vec3.new(xDirNorm, 0.0, zDirNorm)
	if (isScared == true) then 
        if (dirLength < scareDistance) then
            body.velocity.x = scareDistance * body.velocity.x * -1
            body.velocity.z = scareDistance * body.velocity.z * -1
        else
            body.velocity = vec3.new(0.0, 0.0, 0.0)
        end    
    else
        body.velocity = body.velocity * enemySpeed
    end
end

function MeleeAttack()
    -- collision handling
    local transform = gameObject:GetTransform()
	local position = transform.position
    --  acquire target 
    local target = player
    if (target == nil or target.isActive == false) then
        return
    end
    local targetTransform = target:GetTransform()
    local targetPos = targetTransform.position
    
    -- calculate direction
    local xDir = targetPos.x - position.x
	local zDir = targetPos.z - position.z
	local dirLength = math.sqrt(xDir*xDir + zDir*zDir)
	local xDirNorm = xDir / dirLength
	local zDirNorm = zDir / dirLength

    local attack = gameObject:GetAttack()
    if (dirLength < 3 and attack.shouldAttack == true) then
        local impulseStrength = vec3.new(1000, 0, 1000)
        local impulse = vec3.new(xDirNorm, 0.0, zDirNorm)
        impulse  = impulse * impulseStrength
        ApplyLinearImpulse(player, impulse)

        local impulseReverse = vec3.new(-xDirNorm, 0.0, -zDirNorm)
        impulseReverse = impulseReverse * impulseStrength
        ApplyLinearImpulse(gameObject, impulseReverse)
            
        local playerHealth = player:GetHealth()
        playerHealth.hitPoints = playerHealth.hitPoints - 1;
        attack.shouldAttack = false
    end
end

function LookAtThePlayer()
	local transform = gameObject:GetTransform()
	local selfPos = transform.position
	local body = gameObject:GetBody()
    body.velocity = vec3.new(0.0, 0.0, 0.0)
	-- Get Player
	local target = player
    if (target == nil or target.isActive == false) then
        return
    end
    local targetTransform = target:GetTransform()
    local targetPos = targetTransform.position

    -- calculate direction
    local xDir = targetPos.x - selfPos.x
	local zDir = targetPos.z - selfPos.z
	local dirLength = math.sqrt(xDir*xDir + zDir*zDir)
	local xDirNorm = xDir / dirLength
	local zDirNorm = zDir / dirLength
	
    -- look at the target
    local rot = vec3.new(0.0, 0.0, 0.0)
    local tangent = xDirNorm / zDirNorm
    local radians = math.atan(tangent)
    local degree = radians * 180 / math.pi
    if zDirNorm >= 0 then  
	    rot = vec3.new(0.0, degree, 0.0)
        transform:Rotate(rot)
    end
    if zDirNorm < 0 then 
	    rot = vec3.new(0.0, degree + 180, 0.0)
        transform:Rotate(rot)
    end
end

function Charge(currentTime, startTime, endTime)
    local material = gameObject:GetMaterial()
    local redChanel = 5*(currentTime - startTime) / (endTime - startTime)
    material.diffuse = vec3.new(redChanel,0,0)
end

function Cooldown(currentTime, startTime, endTime)
    local material = gameObject:GetMaterial()
    local redChanel = 1 - (currentTime - startTime) / (endTime - startTime)
    material.diffuse = vec3.new(redChanel,0,0)
end

function MoveToCenter()
  	local enemySpeed = 5.0
    
    -- collision handling
    local transform = gameObject:GetTransform()
	local position = transform.position
    --  acquire target 
	local room = GetDungeonFloor(currentFloor):GetRoomFromIndex(currentRoom)
	local roomCoords = room:GetCoords()
    local centerX = (roomCoords.y + roomCoords.y + 1) * roomSize / 2 
    local centerZ = (roomCoords.x + roomCoords.x + 1) * roomSize / 2 
    local targetPos = vec3.new(centerX, position.y, centerZ) 
    
    -- calculate direction
    local xDir = targetPos.x - position.x
	local zDir = targetPos.z - position.z
	local dirLength = math.sqrt(xDir*xDir + zDir*zDir)
	local xDirNorm = xDir / dirLength
	local zDirNorm = zDir / dirLength

    if (dirLength > 0.1) then -- walking to the center
        -- look at the target
        local rot = vec3.new(0.0, 0.0, 0.0)
        local tangent = xDirNorm / zDirNorm
        local radians = math.atan(tangent)
        local degree = radians * 180 / math.pi
        if zDirNorm >= 0 then  
	        rot = vec3.new(0.0, degree, 0.0)
            transform:Rotate(rot)
        end
        if zDirNorm < 0 then 
	        rot = vec3.new(0.0, degree + 180, 0.0)
            transform:Rotate(rot)
        end
    
        -- setting the velocity
        local body = gameObject:GetBody()
        body.velocity = vec3.new(xDirNorm, 0.0, zDirNorm)
	    if (isScared == true) then 
            if (dirLength < scareDistance) then
                body.velocity.x = scareDistance * body.velocity.x * -1
                body.velocity.z = scareDistance * body.velocity.z * -1
            else
                body.velocity = vec3.new(0.0, 0.0, 0.0)
            end    
        else
            body.velocity = body.velocity * enemySpeed
        end
    else -- look at the player
        local body = gameObject:GetBody()
        body.velocity = vec3.new(0.0, 0.0, 0.0)
        LookAtThePlayer()

        -- once at the center signal to attacking script to spawn rocks
        local attack = gameObject:GetAttack()
        if (attack.isFired2 ~= true) then
            attack.shouldAttack2 = true
        end
    end
end

function Update()
    local maxHealth = 200; 
	local health = gameObject:GetHealth()
    local hitPoints = health.hitPoints
    if (hitPoints < maxHealth / 3) then
        MoveToCenter()
    elseif (hitPoints < maxHealth * 2 / 3) then
        local material = gameObject:GetMaterial()
        material.diffuse = vec3.new(0,0,0)
        MoveToCenter()
    else
        local attack = gameObject:GetAttack()
        attack.baseAttackTime = 9
	    if (attack.currentAttackTime > attack.baseAttackTime) then
            attack.currentAttackTime = 0
        elseif (attack.currentAttackTime > attack.baseAttackTime * 2 / 3) then
            Cooldown(attack.currentAttackTime, attack.baseAttackTime * 2 /3, attack.baseAttackTime)
            attack.IsFired = false -- resetting after the RunAtThePlayer
            attack.shouldAttack = true -- resets for MeleeAttack
        elseif (attack.currentAttackTime > attack.baseAttackTime / 3) then
            if (attack.IsFired == false) then
                RunAtThePlayer()
                attack.IsFired = true
                PlaySFX("Resources/Audio/SFX/BossChargingAttack.wav")
            end
            MeleeAttack()
        else
            LookAtThePlayer()
            Charge(attack.currentAttackTime, 0, attack.baseAttackTime / 3)
            --PlaySFX("Resources/Audio/SFX/BossCharging.wav")
        end
        -- Handled By EnemyBoss2Fire.lua, considering cooldown for each charge
    end
end

Update()