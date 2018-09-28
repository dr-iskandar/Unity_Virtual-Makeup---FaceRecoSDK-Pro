using System;
using System.Collections.Generic;
using UnityEngine;
using milook;
public class ContanerPosition : MonoBehaviour
{
	public GameObject contatiner;
	const float pi=(float) 180.0 / Mathf.PI;
	void Awake(){
	}
	void Start(){
	}

	void OnTrack (MilookEngine.TrackerData tdata) {
		if (tdata.error == 0) {
			float offset = Mathf.Max (MilookEngine.instance.GetWidth(), MilookEngine.instance.GetHeight());
			contatiner.transform.localPosition = new Vector3 (tdata.facePose [0]*offset, tdata.facePose [1]*offset, tdata.facePose [2]*offset);
			contatiner.transform.localEulerAngles = new Vector3 (tdata.facePose [3] * pi, tdata.facePose [4] * pi, tdata.facePose [5] * pi);
		}
	}
}
