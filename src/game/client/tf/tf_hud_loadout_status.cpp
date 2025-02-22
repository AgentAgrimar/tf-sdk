#include "cbase.h"
#include "clientmode_tf.h"
#include "vgui_controls/AnimationController.h"
#include "tf_gc_client.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

//
// The HUD element you see in game that says what you're in queue for
//
class CLoadoutHUDStatus : public EditablePanel,
						  public CHudElement
{
	DECLARE_CLASS_SIMPLE( CLoadoutHUDStatus, EditablePanel );

public:
	CLoadoutHUDStatus( const char *pElementName )
		: CHudElement( pElementName )
		, EditablePanel( NULL, "LoadoutHUDStatus" )
	{
		Panel *pParent = g_pClientMode->GetViewport();
		SetParent( pParent );

		vgui::ivgui()->AddTickSignal( GetVPanel() );
	}

	virtual void ApplySchemeSettings( IScheme *pScheme ) OVERRIDE
	{
		BaseClass::ApplySchemeSettings( pScheme );

		LoadControlSettings( "resource/UI/InGameLoadoutStatus.res" );
	}

	virtual void OnTick( void )
	{
		const CTFGCClientSystem::EWebapiInventoryState &eClientState = GTFGCClientSystem()->m_WebapiInventory.m_eState;
		if ( eClientState == m_eUIState )
			return;

		m_eUIState = eClientState;

		wchar_t wszBuff[ 256 ];
		GetLoadoutString( wszBuff, sizeof( wszBuff ) );
		SetDialogVariable( "loadout_state", wszBuff );

		SetAlpha( 100 );
		g_pClientMode->GetViewportAnimationController()->RunAnimationCommand( this, "alpha", 0.f, 0.1f, 2.f, vgui::AnimationController::INTERPOLATOR_DEACCEL );
	}

private:
	void GetLoadoutString( wchar_t* pwszBuff, int nSize )
	{
		const char *pszLocString = "#TF_LoadoutState_Inventory_Initialize";

		if ( m_eUIState < CTFGCClientSystem::EWebapiInventoryState::kWebapiInventoryState_RequestInventory )
			pszLocString = "#TF_LoadoutState_Inventory_Pending";
		else if ( m_eUIState == CTFGCClientSystem::EWebapiInventoryState::kWebapiInventoryState_WaitingForInventory )
			pszLocString = "#TF_LoadoutState_Inventory_Downloading";
		else if ( m_eUIState == CTFGCClientSystem::EWebapiInventoryState::kWebapiInventoryState_InventoryReceived )
			pszLocString = "#TF_LoadoutState_Inventory_Downloaded";
		else if ( m_eUIState == CTFGCClientSystem::EWebapiInventoryState::kWebapiInventoryState_BuildServerMessage )
			pszLocString = "#TF_LoadoutState_Pending";
		else if ( m_eUIState < CTFGCClientSystem::EWebapiInventoryState::kWebapiInventoryState_SentToServer )
			pszLocString = "#TF_LoadoutState_Uploading";
		else if ( m_eUIState == CTFGCClientSystem::EWebapiInventoryState::kWebapiInventoryState_SentToServer )
			pszLocString = "#TF_LoadoutState_Uploaded";
		else
			pszLocString = "#TF_LoadoutState_Updated";

		g_pVGuiLocalize->ConstructString( pwszBuff, 
											  nSize,
											  g_pVGuiLocalize->Find( pszLocString ),
											  0 );
	}

	CTFGCClientSystem::EWebapiInventoryState m_eUIState;
};

DECLARE_HUDELEMENT( CLoadoutHUDStatus );
