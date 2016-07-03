
#include <MyGUI\MyGUI_Prerequest.h>
#include <MyGUI\MyGUI_IResource.h>

namespace FlagRTS
{
	class CursorImageWin32 : public MyGUI::IResource
	{
		MYGUI_RTTI_DERIVED( CursorImageWin32 );

	public:
		CursorImageWin32();
		virtual ~CursorImageWin32() { }

		virtual void deserialization(MyGUI::xml::ElementPtr _node, MyGUI::Version _version);

		size_t GetCursorHandle() const 
		{
			return _handle;
		}

	private:
		size_t _handle;
	};
}