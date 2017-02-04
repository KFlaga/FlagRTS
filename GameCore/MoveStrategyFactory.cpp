#include "DefaultMoveStrategies.h"

namespace FlagRTS
{
	MoveStrategyFactory::MoveStrategyFactory() :
		SubClassXmlFactory("IMoveStrategy")
	{
		RegisterFactory("NoMove", xNew0(FinalClassXmlFactory<NoMoveMoveStrategy>));
		RegisterFactory("NoMove", xNew0(FinalClassXmlFactory<FaceAndGoMoveStrategy>));
		RegisterFactory("NoMove", xNew0(FinalClassXmlFactory<RotateOnlyStrategy>));
	}
}
