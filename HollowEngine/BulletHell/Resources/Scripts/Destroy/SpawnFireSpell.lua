function SpawnFireSpell ()
	local go = CreatePrefabAtPosition("FireSpell", gameObject:GetTransform().position)
	go:GetBody().position.y = player:GetBody().position.y
	go:GetTransform().position.y = go:GetBody().position.y
end

SpawnFireSpell()