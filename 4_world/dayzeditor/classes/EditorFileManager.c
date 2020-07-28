
class EditorWorldObject
{
	string Classname;
	vector Transform[4];
	
	void EditorWorldObject(string classname, vector transform[4]) 
	{
		Classname = classname; Transform = transform;
	}
}

class EditorWorldData
{
	string MapName;
	vector CameraPosition[4];
	ref array<ref EditorWorldObject> WorldObjects;
	
	void EditorWorldData()
	{
		WorldObjects = new array<ref EditorWorldObject>();
	}
	

	
}

enum ExportMode 
{
	TERRAINBUILDER,
	COMFILE, 
	EXPANSION
}


enum HeightType 
{
	ABSOLUTE,
	RELATIVE
}

class ExportSettings
{
	static HeightType ExportHeightType;
}

class EditorFileManager
{

	static void SaveFile(EditorWorldData data, string filename = "$profile:editor_save.txt")
	{
		JsonFileLoader<EditorWorldData>.JsonSaveFile(filename, data);
	}
	
	static EditorWorldData LoadFile(string filename = "$profile:editor_save.txt")
	{
		EditorWorldData data = new EditorWorldData();
		JsonFileLoader<EditorWorldData>.JsonLoadFile(filename, data);
		
		return data;
		
	}
	
	static void ExportToFile(ExportMode mode = ExportMode.TERRAINBUILDER, string filename = "$profile:editor_export.txt", HeightType height_type = HeightType.RELATIVE)
	{
		DeleteFile(filename);
		FileHandle handle = OpenFile(filename, FileMode.WRITE | FileMode.APPEND);
		if (handle == 0) {
			Print("ExportToFile Failed: 0");
			return;
		}
		
		foreach (EditorObject editor_object: Editor.PlacedObjects) {
						
			vector position = editor_object.GetDefaultHitPosition();
			vector orientation = editor_object.GetOrientation();
			orientation = orientation.VectorToAngles();
			float scale = 1;//editor_object.GetScale();
			

			
			
			vector terrainbuilder_offset = Vector(200000, 0, 0);
			string line;
			switch (mode) {
				
				case ExportMode.TERRAINBUILDER: {
					// "construction_house2";206638.935547;6076.024414;146.000015;0.000000;0.000000;1.000000;
					// Name, X, Y, Yaw, Pitch, Roll, Scale, Relative Height
					array<LOD> testlods = new array<LOD>();
					editor_object.GetObject().GetLODS(testlods);
					
					foreach (LOD lod: testlods) {
						Print(editor_object.GetObject().GetLODName(lod));
						array<Selection> selections = new array<Selection>();
						lod.GetSelections(selections);
						foreach (Selection s: selections) {
							
							Print(s.GetName());
							for (int fff = 0; fff < s.GetVertexCount(); fff++) {
								Print(s.GetVertexPosition(lod, fff));
							}
						}
					}
					
					//if (height_type == HeightType.RELATIVE)
						//position[1] = position[1] - GetGame().SurfaceY(position[0], position[2]);
					
					position = position + terrainbuilder_offset;
					line = string.Format("\"%1\";%2;%3;%4;%5;%6;%7;%8;", editor_object.GetModelName(), position[0], position[2], orientation[0], orientation[1], orientation[2], scale, position[1]);
					FPrintln(handle, line);
					break;
				}
					
				case ExportMode.COMFILE: {
					// SpawnObject("Land_Construction_House2", "6638.935547 7.190318 6076.024414", "146.000015 0.000000 0.000000")
					line = string.Format("SpawnObject(\"%1\", \"%2\", \"%3\");", editor_object.GetType(), position.ToString(false), orientation.ToString(false));
					FPrintln(handle, line);
					break;
				}
				
				case ExportMode.EXPANSION: {
					// Land_Construction_House2|13108.842773 10.015385 6931.083984|-101.999985 0.000000 0.000000
					//orientation = orientation.VectorToAngles();
					line = string.Format("%1|%2 %3 %4|%5 %6 %7", editor_object.GetType(), position[0], position[1], position[2], orientation[0], orientation[1], orientation[2]);
					FPrintln(handle, line);
					break;
				}
				
				default: {
					FPrintln(handle, "Line Export Failure");
					break;
				}
				
				
				
			} 
		}
		CloseFile(handle);
	}
}