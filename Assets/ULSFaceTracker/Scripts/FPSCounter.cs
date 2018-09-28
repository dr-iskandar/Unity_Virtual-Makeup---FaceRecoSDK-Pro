using UnityEngine;
using System.Collections;
using UnityEngine.UI;

[RequireComponent(typeof(Text))]
public class FPSCounter : MonoBehaviour {
	public float frequency = 0.2f;
	public int FramesPerSec { get; protected set; }

	Text _uiText;

	// Use this for initialization
	void Start () {
		_uiText = GetComponent<Text> ();
		StartCoroutine(FPS());
	}

		private IEnumerator FPS() {
			for(;;){
				// Capture frame-per-second
				int lastFrameCount = Time.frameCount;
				float lastTime = Time.realtimeSinceStartup;
				yield return new WaitForSeconds(frequency);
				float timeSpan = Time.realtimeSinceStartup - lastTime;
				int frameCount = Time.frameCount - lastFrameCount;

				// Display it
				FramesPerSec = Mathf.RoundToInt(frameCount / timeSpan);
				_uiText.text = FramesPerSec.ToString() + " fps";
			}
		}
	
}
