using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using milook;
public class UI : MonoBehaviour {
	public bool isMainPage;
//	public MilookEngine engine;
	void Start () {
//		Vector3 s = transform.eulerAngles;
//		s.z = MilookEngine.instance.source.getRotation ()-180;
//		transform.eulerAngles = s;
	}
	void OnGUI(){
		if (isMainPage) {
			if (GUI.Button (new Rect (0, 0, 240, 80), "Demo_Accessory")) {
				SceneManager.LoadScene ("Demo_Accessory",LoadSceneMode.Single);
			}
			if (GUI.Button (new Rect (0, 100, 240, 80), "Demo_Avatar_Camera")) {
				SceneManager.LoadScene ("Demo_Avatar_Camera",LoadSceneMode.Single);

			}
			if (GUI.Button (new Rect (0, 200, 240, 80), "Demo_Avatar_Movie")) {
				SceneManager.LoadScene ("Demo_Avatar_Movie",LoadSceneMode.Single);

			}
			if (GUI.Button (new Rect (0, 300, 240, 80), "Demo_Expression")) {
				SceneManager.LoadScene ("Demo_Expression",LoadSceneMode.Single);
			}
			if (GUI.Button (new Rect (0, 400, 240, 80), "Demo_Mask")) {
				SceneManager.LoadScene ("Demo_Mask",LoadSceneMode.Single);
			}
			if (GUI.Button (new Rect (0, 500, 240, 80), "Demo_Wireframe")) {
				SceneManager.LoadScene ("Demo_Wireframe",LoadSceneMode.Single);
			}
		} else {
			if (GUI.Button (new Rect (0, 0, 240, 80), "Back")) {
				SceneManager.LoadScene ("MainPage",LoadSceneMode.Single);
			}
		}

	}
	void OnTrack(MilookEngine.TrackerData trackerData){
	//	if(trackerData.error!=0) return;
	}
}
