#include "Actor/AuraEnemySpawnPoint.h"
#include "Character/AuraEnemy.h"

// spawn a new minion for the shaman
void AAuraEnemySpawnPoint::SpawnEnemy()
{
	AAuraEnemy* Enemy = GetWorld()->SpawnActorDeferred<AAuraEnemy>(this->EnemyClass, GetActorTransform(), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	Enemy->SetEnemyLevel(this->EnemyLevel);
	Enemy->SetCharacterClass(AAuraEnemy::Execute_GetCharacterClass(Enemy));
	Enemy->FinishSpawning(GetActorTransform());
	Enemy->SpawnDefaultController();
}
