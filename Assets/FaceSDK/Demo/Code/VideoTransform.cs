using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using milook;
public class VideoTransform : MonoBehaviour {

	// Use this for initialization
	IEnumerator Start () {
		yield return 1;
		BasicSource m = MilookEngine.instance.source;
		Debug.Log ("Rotation:"+m.getRotation());
		int r =  m.getRotation ()-180;
		Vector3 rot = transform.eulerAngles;
		rot.z = r;
		transform.eulerAngles = rot;
		if (m.getRotation () == 270) {
			Vector3 s = transform.localScale;
			s.y = -s.y;
			transform.localScale = s;
		}
	}
	
	void Update () {
		
	}
}
