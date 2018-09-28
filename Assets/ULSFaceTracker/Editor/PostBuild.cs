using UnityEngine;
using UnityEngine.Rendering;
using UnityEditor;
using UnityEditor.Callbacks;
using System.IO;

public class PostBuild  {
    [PostProcessBuild]
	public static void OnPostprocessBuild (BuildTarget target, string pathToBuiltProject) {
		if (target == BuildTarget.iOS) {

			//Disable metal for the remote display sample.  Metal is not supported due to shaders used in the source Unity sample.
			PlayerSettings.SetUseDefaultGraphicsAPIs(BuildTarget.iOS, false);
			GraphicsDeviceType[] apis = {GraphicsDeviceType.OpenGLES2};
			PlayerSettings.SetGraphicsAPIs(BuildTarget.iOS, apis);

			// We use UnityEditor.iOS.Xcode API which only exists in iOS editor module
			#if UNITY_IOS
			string projPath = pathToBuiltProject + "/Unity-iPhone.xcodeproj/project.pbxproj";
			UnityEditor.iOS.Xcode.PBXProject proj = new UnityEditor.iOS.Xcode.PBXProject();
			proj.ReadFromString(File.ReadAllText(projPath));
			string projTarget = proj.TargetGuidByName("Unity-iPhone");

			var file = "ULSGPUAssets.bin";
			var dstLocalPath = "Frameworks/" + file;
			var dstPath = Path.Combine(pathToBuiltProject, dstLocalPath);
			File.Copy("Assets/ULSFaceTracker/Plugins/iOS/"+file, dstPath, true);
			proj.AddFileToBuild(projTarget, proj.AddFile(dstLocalPath, dstLocalPath));
			File.WriteAllText(projPath, proj.WriteToString());
			#endif
		} else if (target == BuildTarget.StandaloneWindows || target == BuildTarget.StandaloneWindows64) {
			string path = pathToBuiltProject.Substring(0, pathToBuiltProject.IndexOf(".exe")) + "_Data/Plugins/model";
			FileUtil.CopyFileOrDirectory("Assets/ULSFaceTracker/Plugins/model", path);
		}
    }
}
