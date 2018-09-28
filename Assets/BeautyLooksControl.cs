using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BeautyLooksControl : MonoBehaviour {

    public Renderer mask;

    public void changeMask(Texture2D myBeauty)
    {
        mask.material.mainTexture = myBeauty;
    }
}