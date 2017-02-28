#include "CommandRequirement.h"

namespace FlagRTS
{
#define DECLARE_COMMAND_REQUIREMENT(ReqClassName) \
	class ReqClassName : public CommandRequirement \
	{ \
		class Condition : public ICommandCondition \
		{ \
		public: \
			Condition() { } \
			Condition(Command* command) : ICommandCondition(command) { } \
			void SetParameters(uint8 player) { } \
			bool CheckCondition(); \
			ICondition* GetCopy() { return xNew0(Condition); } \
		}; \
	public: \
		ReqClassName(TechAvailability onTrue, TechAvailability onFalse) : \
			CommandRequirement(onTrue, onFalse, xNew0(Condition)) \
		{ } \
		ReqClassName(TechAvailability onTrue, TechAvailability onFalse, Command* command) : \
			CommandRequirement(onTrue, onFalse, xNew1(Condition, command), command) \
		{ } \
		~ReqClassName() { } \
		void SetPlayer(uint8 player) { } \
		TechRequirement* GetCopy() { return xNew2(ReqClassName, _stateOnTrue, _stateOnFalse); } \
		class Factory : public TechRequirementFactory \
		{ \
		public: \
			TechRequirement* Create(XmlNode* node)  \
			{ \
				TechAvailability onTrue = ParseTechAvailability(XmlUtility::XmlGetString(node, "on_true")); \
				TechAvailability onFalse = ParseTechAvailability(XmlUtility::XmlGetString(node, "on_false")); \
				return xNew2(ReqClassName, onTrue, onFalse); \
			} \
		}; \
	}

DECLARE_COMMAND_REQUIREMENT(ExecutorIsConstructedRequirement);
DECLARE_COMMAND_REQUIREMENT(ExecutorIsImmobilizedRequirement);
DECLARE_COMMAND_REQUIREMENT(ExecutorIsDisarmedRequirement);

}