#include "Movers.h"

namespace FlagRTS
{
	MoverFactory::MoverFactory() :
		SubClassXmlFactory("IMover")
	{
		RegisterFactory("Line", xNew0(FinalClassXmlFactory<LineMover>));
		RegisterFactory("AcceleratedLine", xNew0(FinalClassXmlFactory<AcceleratedLineMover>));
	}
}
