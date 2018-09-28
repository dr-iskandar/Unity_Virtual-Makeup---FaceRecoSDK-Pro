using System.Collections;
using System.Collections.Generic;
using UnityEngine;
namespace milook{
	/// <summary>
	/// Pixels data source.
	/// </summary>
	public class BasicSource : MonoBehaviour {

		[HideInInspector]
		public int[] trackRotation=new int[]{270,90,180,180,180};
		[HideInInspector]
		public int[] mirror=new int[]{0,0,0,0,0};
		protected Color32[] pixels;
		[HideInInspector]
		public MilookEngine target;
		protected int width;
		protected int height;
		public Color32[] GetPixels(){
			return pixels;
		}
		 public int GetWidth(){
			return width;
		}
		 public int GetHeight(){
			return height;
		}
		/// <summary>
		/// Get video mirror sig.
		/// </summary>
		public int getMirror(){
			return mirror [platformId];
		}
		/// <summary>
		/// Get video rotation.
		/// </summary>
		public int getRotation(){
			return trackRotation[platformId];
		}
		virtual protected int platformId{
			get{
				#if UNITY_EDITOR
				return 4;
				#elif UNITY_IOS
				return 0;
				#elif UNITY_ANDROID
				return 1;
				#elif UNITY_STANDALONE_OSX
				return 2;
				#elif UNITY_STANDALONE_WIN
				return 3;
				#endif
				}
			}
	}
}