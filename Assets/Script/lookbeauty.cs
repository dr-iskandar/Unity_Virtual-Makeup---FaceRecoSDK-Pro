using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class lookbeauty : MonoBehaviour {

	string urls = "http://45.77.169.132:9000/v1/user/mybeautylooks";
	private string TOKEN = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpZCI6MTczLCJ1c2VybmFtZSI6InNoaW50YXZyIiwicm9sZSI6ImNvbW11bml0eSIsImlhdCI6MTUzMjQxNTU1M30.fw9lwg9_8vIhQjFulZ65PfwP-IAoFZsbb2hjxGDPmuY";

	IEnumerator Start () {
		Dictionary<string, string> headers = new Dictionary<string,string>();
		headers.Add("usertoken", TOKEN);
		WWW www = new WWW(urls, null, headers);
		yield return www;
		Debug.Log (www.text);	
	}

}
