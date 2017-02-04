#include "IEffect.h"
#include "LaunchMissleEffect.h"
#include "InflictDamagaEffect.h"
#include "BaseEffects.h"
#include "SpawnObjectEffect.h"

namespace FlagRTS
{
	EffectFactory::EffectFactory() :
		SubClassXmlFactory("IEffect")
	{
		RegisterFactory("LaunchMissle", xNew0(FinalClassXmlFactory<LaunchMissleEffect>));
		RegisterFactory("InflictDamage", xNew0(FinalClassXmlFactory<InflictDamageEffect>));
		RegisterFactory("Multi", xNew0(FinalClassXmlFactory<MultiEffect>));
		RegisterFactory("SpawnObject", xNew0(FinalClassXmlFactory<SpawnObjectEffect>));
	}
}
