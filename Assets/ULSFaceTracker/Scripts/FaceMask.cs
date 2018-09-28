#define DRAW_MARKERS

using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using ULSTrackerForUnity;
using UnityEngine.SceneManagement;

class FaceMask : MonoBehaviour
{

#if DRAW_MARKERS
	public GameObject marker = null;
	List<GameObject> _marks = new List<GameObject>();
	bool drawMarkers = false;
#endif

	public GameObject faceMask = null;

	float[] _trackPoints = null;

	public Texture2D[] _masks = null;
	int maskTextureIndex = 0;

	private Mesh _mesh = null;
	private Vector3[] _vertices;

	bool initDone = false;

	void createMesh() {
		_trackPoints = new float[Plugins.MAX_TRACKER_POINTS*2];
		MeshFilter filter = faceMask.GetComponent<MeshFilter>();
		_mesh = filter.mesh;
		if (_mesh == null) {
			_mesh = new Mesh();
			filter.mesh = _mesh;
		}
		_vertices = _mesh.vertices;
	}

	void Start ()
	{
		InitializeTrackerAndCheckKey();
		createMesh ();
		Application.targetFrameRate = 60;

#if DRAW_MARKERS
		for (int i=0; i< Plugins.MAX_TRACKER_POINTS; ++i) {
			var g = Instantiate (marker);
			g.transform.parent = transform.parent;
			g.SetActive (false);
			_marks.Add (g);
		}
#endif
	}

	// Initialize tracker and check activation key
	void InitializeTrackerAndCheckKey ()
	{
		Plugins.OnPreviewStart = initCameraTexture;
		//Plugins.OnPreviewUpdate = previewUpdate;

		int initTracker = Plugins.ULS_UnityTrackerInit();
		if (initTracker < 0) {
			Debug.Log ("Failed to initialize tracker.");
		} else {
			Debug.Log ("Tracker initialization succeeded");
		}
	}
		
	void initCameraTexture (Texture preview, int rotate) {
		int w = preview.width;
		int h = preview.height;
		GetComponent<Renderer> ().material.mainTexture = preview;

		// adjust scale and position to map tracker points
#if UNITY_STANDALONE || UNITY_EDITOR
		// adjust scale and position to map tracker points
		transform.localScale = new Vector3 (w, h, 1);
		transform.localPosition = new Vector3 (w/2, h/2, 1);
		transform.parent.localScale = new Vector3 (-1, -1, 1);
		transform.parent.localPosition = new Vector3 (w/2, h/2, 0);
		Camera.main.orthographicSize = h / 2;
#elif UNITY_IOS || UNITY_ANDROID
		int orthographicSize = w / 2;
		if (Screen.orientation == ScreenOrientation.LandscapeLeft || Screen.orientation == ScreenOrientation.LandscapeRight) {
			orthographicSize = h / 2;
		}

		transform.localScale = new Vector3 (w, h, 1);
		transform.localPosition = new Vector3 (w/2, h/2, 1);//anchor: left-bottom
		transform.parent.localPosition = Vector3.zero; //reset position for rotation
		transform.parent.transform.eulerAngles = new Vector3 (0, 0, rotate);//orientation
		transform.parent.localPosition = transform.parent.transform.TransformPoint(-transform.localPosition); //move to center
		Camera.main.orthographicSize = orthographicSize;
#endif

		initDone = true;
	}

	void Update () {
		if (!initDone)	
			return;

        // Show tracking result
        if (0 < Plugins.ULS_UnityGetPoints (_trackPoints)) {
			for (int j = 0; j < Plugins.MAX_TRACKER_POINTS; ++j) {
				_vertices [j] = new Vector3 (_trackPoints [j * 2], _trackPoints [j * 2 + 1], 0);
			}
			_mesh.MarkDynamic ();
			_mesh.vertices = _vertices;
			_mesh.RecalculateBounds ();
			faceMask.SetActive (true);

#if DRAW_MARKERS
			for(int j=0;j<Plugins.MAX_TRACKER_POINTS;++j) {
				int v = j*2;
				Vector3 pt = new Vector3(_trackPoints[v],_trackPoints[v+1], 0);
				_marks[j].transform.localPosition = (pt);
				_marks[j].SetActive(drawMarkers);
			}
#endif
		} else {
			faceMask.SetActive (false);
		}
	}

	bool frontal = true;
	bool flashLight = false;
	bool pause = false;
	bool enableTracker = true;

    public void ChangeColor(Texture2D newcol)
    {
        faceMask.GetComponent<Renderer>().material.mainTexture = newcol;
    }

    public void SwitchCamera()
    {
        frontal = !frontal;
          if(frontal)
              Plugins.ULS_UnitySetupCamera (640, 480, 30, true);
          else
              Plugins.ULS_UnitySetupCamera (1280, 720, 60, false);
    }

//	void OnGUI() {
//#if DRAW_MARKERS
//		if (GUILayout.Button ("Show Markers", GUILayout.Height (80))) {
//			drawMarkers ^= true;
//		}
//		GUILayout.Space (8);
//#endif

//		if (GUILayout.Button ("Next Mask", GUILayout.Height (80))) {
//			maskTextureIndex = (maskTextureIndex + 1) % _masks.Length;
//			faceMask.GetComponent<Renderer> ().material.mainTexture = _masks [maskTextureIndex];
//		}
//		GUILayout.Space (8);

//		if (GUILayout.Button ("Toggle Tracker", GUILayout.Height (100))) {
//			enableTracker ^= true;
//			Plugins.ULS_UnityTrackerEnable (enableTracker);
//		}
//		GUILayout.Space (8);

//#if UNITY_STANDALONE || UNITY_EDITOR
//#else
//		if (GUILayout.Button ("Switch Camera", GUILayout.Height (80))) {
//			
//		}
//		GUILayout.Space (8);
//		if (GUILayout.Button ("Toggle FlashLight", GUILayout.Height (80))) {
//		flashLight = !flashLight;
//		Plugins.ULS_UnitySetFlashLight (flashLight);
//		}
//		GUILayout.Space (8);
//#endif

	//	if (GUILayout.Button ("Toggle Camera", GUILayout.Height (80))) {
	//		pause = !pause;
	//		Plugins.ULS_UnityPauseCamera (pause);
	//	}
	//	GUILayout.Space (8);

	//	if (GUILayout.Button ("Change Scene", GUILayout.Height (80))) {
	//		Plugins.ULS_UnityTrackerTerminate ();
	//		Debug.Log ("Change Scene");
	//		SceneManager.LoadScene ("Object3D");
	//	}

	//}
}