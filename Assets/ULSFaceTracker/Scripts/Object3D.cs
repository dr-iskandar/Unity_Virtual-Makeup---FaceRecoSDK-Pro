#define DRAW_MARKERS

using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using ULSTrackerForUnity;
using UnityEngine.SceneManagement;

class Object3D : MonoBehaviour
{

#if DRAW_MARKERS
	public GameObject marker = null;
	List<GameObject> _marker2d = new List<GameObject>();
	bool drawMarkers = false;
#endif

	float[] _trackPoints = new float[Plugins.MAX_TRACKER_POINTS*2];
	float[] _scaleX = new float[1];
	float[] _scaleY = new float[1];
	float[] _scaleZ = new float[1];
	float[] _fovx = new float[1];
	float[] _fovy = new float[1];
	float[] _mtx = new float[16];
	Matrix4x4 transformationM = Matrix4x4.identity, adjustMatrix, ARM;

	bool initDone = false;


	[Range(1,10000)]
	public float _focal_length = 5120; // assumption for focal length

	public GameObject _anchor2d = null;
	public GameObject _anchor3d = null;
	public Camera ARCamera = null;
	Transform _helmet = null;
	Vector3 _original_scale;

	// intrinsic camera matrix of camera
	float[] intrinsic_camera_matrix = new float[] {
		5120, 0f, 320f,
		0f, 5120, 240f,
		0f, 0f, 1f};

	//distortion coefficients of camera
	//float[] distort_coeffs = new float[] {8.9497368953298462e-02f, -3.8742309493295973e-01f, 0, 0, 4.1841034436731994e-01f};

	void Start ()
	{
		_helmet = _anchor3d.transform.GetChild (0);
		_original_scale = _helmet.transform.localScale;

#if DRAW_MARKERS
		for (int i=0; i< Plugins.MAX_TRACKER_POINTS; ++i) {
			var g = Instantiate (marker);
			g.transform.parent = _anchor2d.transform;	
			g.transform.localScale = new Vector3(5,5,5);
			g.SetActive (false);
			_marker2d.Add (g);
		}
#endif
	
		InitializeTrackerAndCheckKey();
		Application.targetFrameRate = 60;
	}

	// Initialize tracker and check activation key
	void InitializeTrackerAndCheckKey ()
	{
		// setup tracker callback functions
		Plugins.OnPreviewStart = initCameraTexture;
		//Plugins.OnPreviewUpdate = previewUpdate;

		int initTracker = Plugins.ULS_UnityTrackerInit();
		if (initTracker < 0) {
			Debug.Log ("Failed to initialize tracker.");
		} else {
			Debug.Log ("Tracker initialization succeeded");
		}
	}

	//<summary>
	/// callback function for tracker update
	///</summary>
	///<param name="preview"> Input Texture of camera preview</param>
	///
	void previewUpdate(Texture preview) {
		//Debug.Log ("preview update.");
	}

	//<summary>
	/// callback function for setup camera texture and AR matrix at the first time
	///</summary>
	///<param name="preview"> Input Texture of camera preview</param>
	///<param name="rotate"> Input camera rotation in degree. </param>
	///
	void initCameraTexture (Texture preview, int rotate) {
		int w = preview.width;
		int h = preview.height;
		GetComponent<Renderer> ().material.mainTexture = preview;

		// mockup an intrinsic camera matrix
		intrinsic_camera_matrix[0] = _focal_length;
		intrinsic_camera_matrix[4] = _focal_length;
		intrinsic_camera_matrix[2] = w/2;
		intrinsic_camera_matrix[5] = h/2;

		// get FOV for AR camera by calibration function
		Plugins.ULS_UnityCalibration (intrinsic_camera_matrix, w, h, _fovx, _fovy);

		//Debug.Log ("fovx:" + _fovx [0] + ",fovy:" + _fovy [0]);
		//Debug.Log ("screen width:" + Screen.width + ",height:" + Screen.height);
		//Debug.Log ("camera width:" + w + ",height:" + h);
		//Debug.Log ("rotate: "+rotate);

		// adjust transforom for mapping tracker points
#if UNITY_STANDALONE || UNITY_EDITOR
		// adjust scale and position to map tracker points
		transform.localScale = new Vector3 (w, h, 1);
		transform.localPosition = new Vector3 (w/2, h/2, 1);
		transform.parent.localScale = new Vector3 (-1, -1, 1);
		transform.parent.localPosition = new Vector3 (w/2, h/2, 0);
		Camera.main.orthographicSize = h / 2;
		ARCamera.fieldOfView = _fovy[0];
		adjustMatrix = Matrix4x4.TRS (Vector3.zero, Quaternion.identity, new Vector3 (-1, -1, 1));

#elif UNITY_IOS || UNITY_ANDROID
		transform.localScale = new Vector3 (w, h, 1);
		transform.localPosition = new Vector3 (w/2, h/2, 1); //anchor: left-bottom
		transform.parent.localPosition = Vector3.zero; //reset position for rotation
		transform.parent.transform.eulerAngles = new Vector3 (0, 0, rotate); //orientation
		transform.parent.localPosition = transform.parent.transform.TransformPoint(-transform.localPosition); //move to center

		if (Screen.orientation == ScreenOrientation.LandscapeLeft || Screen.orientation == ScreenOrientation.LandscapeRight) {
			Camera.main.orthographicSize = h/2;			
			float v =  (float)(w * Screen.height) / (h * Screen.width);
			ARCamera.rect = new Rect ((1-v)*0.5f, 0, v, 1); // AR viewport
			ARCamera.fieldOfView = _fovy[0];
		} else {
			float aspect = ((float)Screen.height)/((float)Screen.width)*h/w;
			float v = 1f / aspect;
			Camera.main.orthographicSize = w/2 * aspect; 
			ARCamera.rect = new Rect (0, (1-v)*0.5f, 1, v); // AR viewport
			ARCamera.fieldOfView = _fovx[0];
		}

		adjustMatrix = Matrix4x4.TRS (Vector3.zero, Quaternion.Euler(new Vector3(0,0,rotate)), new Vector3 (1, 1, 1));
#endif

		initDone = true;
	}

	void Update () {
		if (!initDone)	
			return;

        // Show tracking result
		if (0 < Plugins.ULS_UnityGetPoints (_trackPoints)) {
#if DRAW_MARKERS
			for(int j=0;j<Plugins.MAX_TRACKER_POINTS;++j) {
				_marker2d[j].transform.localPosition = new Vector3 (_trackPoints [j * 2], _trackPoints [j * 2 + 1], 0);
				_marker2d[j].SetActive(drawMarkers);
			}		
#endif
			// get transform matrix for alignment 3d objects
			Plugins.ULS_UnityGetTransform (_mtx, intrinsic_camera_matrix, null);
			for (int i=0;i<16;++i) transformationM[i] = _mtx[i];
			ARM = adjustMatrix * transformationM;

			// apply alignment matrix to object's transform
			ARUtils.SetTransformFromMatrix(_anchor3d.transform, ref ARM);

			// apply local scale to fit user's face
			Plugins.ULS_UnityGetScale3D (_scaleX,_scaleY,_scaleZ);
			Vector3 s = new Vector3 (_scaleX [0], _scaleY [0], _scaleZ [0]);
			s.Scale (_original_scale);
			_helmet.transform.localScale = s;
		}
	}

	bool frontal = true;

	void OnGUI() {
#if DRAW_MARKERS
		if (GUILayout.Button ("Show Markers", GUILayout.Height (100))) {
			drawMarkers ^= true;
			_helmet.gameObject.SetActive(!drawMarkers);
		}
		GUILayout.Space (8);
#endif

#if UNITY_STANDALONE || UNITY_EDITOR
#else
		if (GUILayout.Button ("Switch Camera", GUILayout.Height (80))) {
			frontal = !frontal;
			if(frontal)
				Plugins.ULS_UnitySetupCamera (640, 480, 30, true);
			else
				Plugins.ULS_UnitySetupCamera (1280, 720, 60, false);
		}
#endif

		GUILayout.Space (8);
		if (GUILayout.Button ("Change Scene", GUILayout.Height (80))) {
			Plugins.ULS_UnityTrackerTerminate ();
			SceneManager.LoadScene ("faceMask");
		}
	}
}