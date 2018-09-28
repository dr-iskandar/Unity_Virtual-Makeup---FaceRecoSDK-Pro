using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;

public class HeaderTes : MonoBehaviour {

	private string URL = "http://45.77.169.132:9000/v1/user/mybeautylooks";
	private string screenShotURL = "http://45.77.169.132:9000/v1/beautylook/post";
	private string TOKEN = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpZCI6MTczLCJ1c2VybmFtZSI6InNoaW50YXZyIiwicm9sZSI6ImNvbW11bml0eSIsImlhdCI6MTUzMjQxNTU1M30.fw9lwg9_8vIhQjFulZ65PfwP-IAoFZsbb2hjxGDPmuY";

    void Start () {	
		//Dictionary<string, string> headers = new Dictionary<string,string>();
		//headers.Add("usertoken", TOKEN);
		//WWW www = new WWW(URL, null, headers);
		//yield return www;


		
		StartCoroutine (UploadPNG ());
	}

	IEnumerator UploadPNG() {
		// We should only read the screen after all rendering is complete
		yield return new WaitForEndOfFrame();

		// Create a texture the size of the screen, RGB24 format
		int width = Screen.width;
		int height = Screen.height;
		var tex = new Texture2D( width, height, TextureFormat.RGB24, false );

		// Read screen contents into the texture
		tex.ReadPixels( new Rect(0, 0, width, height), 0, 0 );
		tex.Apply();

		// Encode texture into PNG
		byte[] bytes = tex.EncodeToPNG();
		Destroy( tex );



		// Create a Web Form
		WWWForm form = new WWWForm();

		form.AddBinaryData("image", bytes, "screenShot.png", "image/png");

        byte[] rawData = form.data;

        Dictionary<string, string> headers = new Dictionary<string, string>();
        headers.Add("usertoken", TOKEN);
        headers.Add("Content-Type", "application/x-www-form-urlencoded");


        WWW www = new WWW(screenShotURL, rawData, headers);
        yield return www;

		// Upload to a cgi script
		using (var w = UnityWebRequest.Post(screenShotURL, form))
		{
			yield return w.SendWebRequest();
			if (w.isNetworkError || w.isHttpError) {
				print(w.error);
                Debug.Log (www.text); 
			}
			else {
				print("Finished Uploading Screenshot");
                Debug.Log (www.text);   
			
			}
		}
			
	}
}