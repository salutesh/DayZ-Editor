

modded class InGameMenu
{
	
	
	override Widget Init()
	{		
		layoutRoot = super.Init();
		
		
		m_RestartDeadButton.Show(false);
		ButtonSetText(m_RestartButton, "CHANGE MAP");
		//ButtonSetText(m_RestartDeadButton, "OPEN FILE");
		ButtonSetText(m_ContinueButton, "CONTINUE");
		
		WrapSpacerWidget top = WrapSpacerWidget.Cast(layoutRoot.FindAnyWidget("top"));		
		top.AddChildAfter(m_OptionsButton, m_RestartButton);
		
		ImageWidget dayz_logo = ImageWidget.Cast(layoutRoot.FindAnyWidget("dayz_logo"));
		dayz_logo.LoadImageFile(0, "DayZEditor/gui/images/logo_editor_big.edds");
		dayz_logo.SetImage(0);
		dayz_logo.SetFlags(dayz_logo.GetFlags() | WidgetFlags.SOURCEALPHA | WidgetFlags.BLEND | WidgetFlags.STRETCH);
		
		return layoutRoot;
	}
	
	
	override void UpdateGUI() {	}
	
	
	override void OnClick_Restart()
	{
		MapSelectDialog select_window = new MapSelectDialog();
		select_window.ShowDialog();
		//GetGame().GetUIManager().ShowScriptedMenu(select_window, this);
	}
	
	

}