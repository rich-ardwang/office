// dllmain.h : Declaration of module class.

class CHeliosModule : public ATL::CAtlDllModuleT< CHeliosModule >
{
public :
	DECLARE_LIBID(LIBID_HeliosLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_HELIOS, "{6E2C3D71-1C6D-43F5-8C10-A31C0BE90C97}")
};

extern class CHeliosModule _AtlModule;
