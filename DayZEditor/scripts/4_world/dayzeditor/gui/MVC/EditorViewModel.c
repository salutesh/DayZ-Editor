
class ObservableType<Class T1>
{
	private T1 _value;
	
	void Set(T1 value)
	{
		_value = value;
		
	}
}



static ref EditorHudViewModel m_EditorHudViewModel;
EditorHudViewModel GetEditorHudViewModel() { return m_EditorHudViewModel; }

// 	  vvvvvvvvvvvvvvvvv  put THIS into ScriptClass
class EditorHudViewModel: ViewModelBase
{
	private ref EditorPlaceableListItemSet m_PlaceableObjects;

	string DebugText1;
	string DebugText2;
	string DebugText3;
	string DebugText4;
	string DebugText5;
	
	bool BuildingSelect;
	bool VehicleSelect;
	bool EntitySelect;
	bool HumanSelect;
	
	bool LeftbarHide;
	bool RightbarHide;
	
	float BrushRadius;
	float BrushDensity;
	
	ref TextListboxWidgetData DebugActionStackListbox;
	ref WrapSpacerWidgetData LeftbarSpacer;
	ref WrapSpacerWidgetData RightbarSpacer;
	ref XComboBoxWidgetData BrushTypeBox;
	
	void EditorHudViewModel()
	{
		EditorLog.Trace("EditorHudViewModel");
		m_EditorHudViewModel = this;
	}
	
	override void OnWidgetScriptInit(Widget w)
	{
		super.OnWidgetScriptInit(w);
		
		DebugActionStackListbox 	= new TextListboxWidgetData("DebugActionStackListbox");
		LeftbarSpacer 				= new WrapSpacerWidgetData("LeftbarSpacer");
		RightbarSpacer 				= new WrapSpacerWidgetData("RightbarSpacer");
		BrushTypeBox				= new XComboBoxWidgetData("BrushTypeBox");
		
		// Load Brushes
		m_EditorBrushTypes = new EditorBrushDataSet();
		m_CustomBrushList = new map<string, typename>();
		ReloadBrushes("$profile:Editor/EditorBrushes.xml");
		RegisterCustomBrush("Delete", DeleteBrush);
	}
	
	// Brush Management
	private ref EditorBrushDataSet 		m_EditorBrushTypes;
	private ref map<string, typename> 	m_CustomBrushList;
	void ReloadBrushes(string filename)
	{
		EditorLog.Trace("EditorHudViewModel::ReloadBrushes");
		XMLEditorBrushes xml_brushes = new XMLEditorBrushes();
		GetXMLApi().Read(filename, xml_brushes);
	}
	
	EditorBrushData GetLoadedBrushData(string name) { return m_EditorBrushTypes.Get(name); }
	void RegisterCustomBrush(string name, typename type) { m_CustomBrushList.Insert(name, type); }
	
	void SetBrushTypes(EditorBrushDataSet brush_types)
	{
		EditorLog.Trace("EditorHudViewModel::SetBrushTypes");
		ClearBrushBox();
		m_EditorBrushTypes = brush_types;

		foreach (EditorBrushData brush: m_EditorBrushTypes)
			InsertBrush(brush.Name);		
	}
		
	EditorBrush GetBrush(string brush_name)
	{
		EditorLog.Trace("EditorSettings::GetBrush " + brush_name);
		foreach (EditorBrushData settings: m_EditorBrushTypes) {
			if (settings.Name == brush_name) {
				foreach (string name, typename type: m_CustomBrushList) 
					if (name == brush_name)
						return type.Spawn();
					
				return new EditorBrush(settings);
			}
		}
		
		EditorLog.Trace("EditorSettings::GetBrush Brush not found!");
		return null;
	}
		

	
	void InsertPlaceableObject(EditorListItem target)
	{
		EditorLog.Trace("EditorHudViewModel::InsertPlaceableObject");
		LeftbarSpacer.Insert(target.GetRoot());
	}	
	
	void InsertPlacedObject(EditorListItem target)
	{
		EditorLog.Trace("EditorHudViewModel::InsertPlacedObject");
		RightbarSpacer.Insert(target.GetRoot());
	}
	
	void InsertMapObject(Widget map_marker)
	{
		Print("EditorHudViewModel::InsertMapObject " + map_marker.GetName());
		//m_EditorMapWidget.AddChild(map_marker);
	}
	
	int ReloadPlaceableObjects() 
	{ 
		m_PlaceableObjects = new EditorPlaceableListItemSet();
		TStringArray paths = new TStringArray;
		paths.Insert(CFG_VEHICLESPATH);
		for (int i = 0; i < paths.Count(); i++)	{
			string Config_Path = paths.Get(i);			
			
		    for (int j = 0; j < GetGame().ConfigGetChildrenCount(Config_Path); j++) {
				string Config_Name, Base_Name;
		        GetGame().ConfigGetChildName(Config_Path, j, Config_Name);
				EditorPlaceableObjectData placeable_object_data = new EditorPlaceableObjectData(Config_Name, Config_Path);
				EditorListItem item = new EditorPlaceableListItem(placeable_object_data);
				m_PlaceableObjects.Insert(item);
				InsertPlaceableObject(item);
				
		    }
		}
		
		if (j == 0) {
			InsertPlaceableObject(new EditorPlaceableListItem(EditorPlaceableObjectData("Dummy_Name", "Dummy_Path")));
		}
		return j;
	}
	
	void UpdatePlaceableItems(PlaceableObjectCategory category)
	{
		EditorLog.Trace("EditorUIViewModel::UpdatePlaceableItems");
		foreach (EditorPlaceableListItem placeable_object: m_PlaceableObjects) {
			Widget root = placeable_object.GetRoot();
			root.Show(placeable_object.GetData().GetCategory() == category);
		}
		
	}
	
	
	void ClearBrushBox()
	{
		BrushTypeBox.Clear();
	}
	
	ref map<int, string> m_TempBrushDir = new map<int, string>();
	private int tempb = 0;
	void InsertBrush(string name)
	{
		m_TempBrushDir.Insert(tempb, name);
		tempb++;
		BrushTypeBox.Insert(name);
	}
	
	override void NotifyPropertyChanged(string property_name = "")
	{
		switch (property_name) {
			
			case "BuildingSelect": {
				VehicleSelect = false; EntitySelect = false; HumanSelect = false;
				UpdatePlaceableItems(PlaceableObjectCategory.BUILDING);
				break;
			}
			
			case "VehicleSelect": {
				BuildingSelect = false; EntitySelect = false; HumanSelect = false;
				UpdatePlaceableItems(PlaceableObjectCategory.VEHICLE);
				break;
			}
			
			case "EntitySelect": {
				BuildingSelect = false; VehicleSelect = false; HumanSelect = false;
				UpdatePlaceableItems(PlaceableObjectCategory.ENTITY);
				break;
			}
			
			case "HumanSelect": {
				BuildingSelect = false; VehicleSelect = false; EntitySelect = false;
				UpdatePlaceableItems(PlaceableObjectCategory.HUMAN);
				break;
			}
			
			case "BrushRadius": {
				EditorBrush.SetRadius(BrushRadius);
				break;
			}
			
			case "BrushDensity": {
				EditorBrush.SetDensity(BrushDensity);
				break;
			}
		}
		
		super.NotifyPropertyChanged(property_name);
	}

}
	


typedef ref map<string, typename> VariableHashMap;


// abstract
class ViewModelBase: Managed
{
	protected Widget m_LayoutRoot;
	
	// Hashed Variable Data
	protected ref VariableHashMap m_ModelHashMap;
	VariableHashMap GetVariableHashMap() { return m_ModelHashMap; }
	typename GetVariableType(string var_name) { return m_ModelHashMap.Get(var_name); }
	
	// View List
	protected ref map<string, ref EditorView> m_ViewList;
	
	void ViewModelBase() { EditorLog.Trace("ViewModelBase"); }
	void ~ViewModelBase() { EditorLog.Trace("~ViewModelBase"); }
	
		
	void OnWidgetScriptInit(Widget w)
	{
		EditorLog.Trace("ViewModelBase::OnWidgetScriptInit");	
		m_LayoutRoot = w;
		
		m_ModelHashMap = new VariableHashMap();
		m_ViewList = new map<string, ref EditorView>();

		typename vtype = Type();
		int vcnt = vtype.GetVariableCount();
		for (int i = 0; i < vcnt; i++)
			m_ModelHashMap.Insert(vtype.GetVariableName(i), vtype.GetVariableType(i));
		
		
	}
	


	void InsertView(string variable_name, ref EditorView view)
	{
		EditorLog.Trace("ViewModelBase::InsertView: " + variable_name);
		m_ViewList.Insert(variable_name, view);
	}
	
	void DebugPrint()
	{
		EditorLog.Debug("ViewModelBase::DebugPrint: " + m_LayoutRoot.GetName());
		foreach (string name, ref EditorView view: m_ViewList)
			view.DebugPrint();
	}
	
	// property_name = name of variable being changed
	ref ScriptInvoker PropertyChanged = new ScriptInvoker();
	void NotifyOnPropertyChanged(func action)
	{	
		if (PropertyChanged == null)
			PropertyChanged = new ScriptInvoker();
		
		PropertyChanged.Insert(action);
	}
	
	void NotifyPropertyChanged(string property_name = "") 
	{
		EditorLog.Trace("ViewModelBase::NotifyPropertyChanged: " + property_name);
		if (property_name == string.Empty) {
			foreach (ref EditorView view: m_ViewList)
				view.UpdateView();
			return;
		}
		
			
		PropertyChanged.Invoke(property_name);
	}
	

	
	bool OnClick(Widget w, int x, int y, bool button) { return true; }
}



