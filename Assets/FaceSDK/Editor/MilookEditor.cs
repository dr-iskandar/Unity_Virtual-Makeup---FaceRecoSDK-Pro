using UnityEditor;
using UnityEngine;
using milook;
//

public class MilookEditor : Editor 
{   
	BasicSource _target; 
	#if UNITY_EDITOR
	int selectIdx=4;
	#elif UNITY_IOS
	int selectIdx=0;
	#elif UNITY_ANDROID
	int selectIdx=1;
	#elif UNITY_STANDALONE_OSX
	int selectIdx=2;
	#elif UNITY_STANDALONE_WIN
	int selectIdx=3;
	#endif
	public override void OnInspectorGUI() 
	{
		base.OnInspectorGUI ();
		_target = (BasicSource) target;   
		if (EditorTools.DrawHeader ("Tracker params")) {
			selectIdx = EditorGUILayout.Popup (selectIdx, new string[]{ "iOS", "Android", "Mac","Windows","Edtor" });
			EditorTools.BeginContents ();
			if (_target.mirror == null || _target.mirror.Length < 5) {
				reset ();
			}
			if (_target.trackRotation == null || _target.trackRotation.Length < 5) {
				reset ();
			}
			_target.mirror[selectIdx] =EditorGUILayout.Toggle ("Mirror",_target.mirror[selectIdx]==1)?1:0;

			EditorGUILayout.BeginHorizontal ();
			EditorGUILayout.LabelField ("Rotation:",GUILayout.Width(50));
			float tempValue = GUILayout.HorizontalSlider ((float)_target.trackRotation [selectIdx], 0.0f, 360.0f);
			_target.trackRotation [selectIdx] = (int)tempValue;
			_target.trackRotation [selectIdx] =(int)Mathf.Floor(tempValue/90)*90;
			EditorGUILayout.LabelField (_target.trackRotation [selectIdx].ToString(),GUILayout.Width(50));
			EditorGUILayout.EndHorizontal ();
		}
		if (GUILayout.Button ("Reset")) {
			reset ();
		}
	}

	private void reset(){
		_target.trackRotation =new int[]{270,90,180,180,180};
		_target.mirror=new int[]{0,0,0,0,0};
	}
}