// Copyright Spectrelight Studios, LLC


#include "SLTilemapEditorBPFL.h"

#if WITH_EDITOR
#include "LevelEditorViewport.h"


bool USLTilemapEditorBPFL::GetMouseVectorInViewport(FVector& OutWorldOrigin, FVector& OutWorldDirection)
{
	if (GCurrentLevelEditingViewportClient)
	{
		FViewport* Viewport = GCurrentLevelEditingViewportClient->Viewport;
		FIntPoint MousePos;
		Viewport->GetMousePos(MousePos);

		FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(Viewport, GCurrentLevelEditingViewportClient->GetScene(), GCurrentLevelEditingViewportClient->EngineShowFlags));

		FSceneView* SceneView = GCurrentLevelEditingViewportClient->CalcSceneView(&ViewFamily);

		FViewportCursorLocation Cursor(SceneView, GCurrentLevelEditingViewportClient, MousePos.X, MousePos.Y);
		OutWorldOrigin = Cursor.GetOrigin();
		OutWorldDirection = Cursor.GetDirection();
		return true;
	}
	return false;
}

#endif
