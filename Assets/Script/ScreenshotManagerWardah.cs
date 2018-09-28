#pragma warning disable 0168 // variable declared but not used.
#pragma warning disable 0219 // variable assigned but not used.


using UnityEngine;
using System;
using System.IO;
using System.Collections;
using System.Runtime.InteropServices;

public class ScreenshotManagerWardah : MonoBehaviour
{

    public static event Action ScreenshotFinishedSaving;
    public static event Action ImageFinishedSaving;


#if UNITY_IPHONE
 
 [DllImport("__Internal")]
    private static extern bool saveToGallery( string path );
 
#endif




    public void saveGallery()
    {
        StartCoroutine(Save("MyBeauty", "Wardah", true));
    }




    public static IEnumerator Save(string fileName, string albumName = "Wardah", bool callback = false)
    {
        bool photoSaved = false;

        ScreenshotManagerWardah.ScreenShotNumber++;

        string screenshotFilename = fileName + "_" + ScreenshotManagerWardah.ScreenShotNumber + ".png";

        Debug.Log("Save screenshot " + screenshotFilename);

#if UNITY_IPHONE
  
   if(Application.platform == RuntimePlatform.IPhonePlayer)
   {
    Debug.Log("iOS platform detected");
    
    string iosPath = Application.persistentDataPath + "/" + screenshotFilename;
  
    Application.CaptureScreenshot(screenshotFilename);
    
    while(!photoSaved)
    {
     photoSaved = saveToGallery( iosPath );
     
     yield return new WaitForSeconds(.5f);
    }
   
    UnityEngine.iOS.Device.SetNoBackupFlag( iosPath );
   
   } else {
   
    Application.CaptureScreenshot(screenshotFilename);
   
   }
   
#elif UNITY_ANDROID

        if (Application.platform == RuntimePlatform.Android)
        {
            Debug.Log("Android platform detected");

            string androidPath = "/../../../../DCIM/" + albumName + "/" + screenshotFilename;
            string path = Application.persistentDataPath + androidPath;
            string pathonly = Path.GetDirectoryName(path);
            Directory.CreateDirectory(pathonly);
            ScreenCapture.CaptureScreenshot(androidPath);

            AndroidJavaClass obj = new AndroidJavaClass("com.ryanwebb.androidscreenshot.MainActivity");

            while (!photoSaved)
            {
                photoSaved = obj.CallStatic<bool>("scanMedia", path);

                yield return new WaitForSeconds(.5f);
            }

        }
        else
        {

            ScreenCapture.CaptureScreenshot(screenshotFilename);

        }
#else
   
   while(!photoSaved)
   {
    yield return new WaitForSeconds(.5f);
  
    Debug.Log("Screenshots only available in iOS/Android mode!");
   
    photoSaved = true;
   }
  
#endif

        if (callback)
            ScreenshotFinishedSaving();
    }


    public static IEnumerator SaveExisting(string filePath, bool callback = false)
    {
        bool photoSaved = false;

        Debug.Log("Save existing file to gallery " + filePath);

#if UNITY_IPHONE
  
   if(Application.platform == RuntimePlatform.IPhonePlayer)
   {
    Debug.Log("iOS platform detected");
    
    while(!photoSaved)
    {
     photoSaved = saveToGallery( filePath );
     
     yield return new WaitForSeconds(.5f);
    }
   
    UnityEngine.iOS.Device.SetNoBackupFlag( filePath );
   }
   
#elif UNITY_ANDROID

        if (Application.platform == RuntimePlatform.Android)
        {
            Debug.Log("Android platform detected");

            AndroidJavaClass obj = new AndroidJavaClass("com.ryanwebb.androidscreenshot.MainActivity");

            while (!photoSaved)
            {
                photoSaved = obj.CallStatic<bool>("scanMedia", filePath);

                yield return new WaitForSeconds(.5f);
            }

        }

#else
   
   while(!photoSaved)
   {
    yield return new WaitForSeconds(.5f);
  
    Debug.Log("Save existing file only available in iOS/Android mode!");

    photoSaved = true;
   }
  
#endif

        if (callback)
            ImageFinishedSaving();



    }


    public static int ScreenShotNumber
    {
        set { PlayerPrefs.SetInt("screenShotNumber", value); }

        get { return PlayerPrefs.GetInt("screenShotNumber"); }
    }
}