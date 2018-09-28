using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.Runtime.InteropServices;
using System.Text;
using UnityEngine.UI;

public class changeImage : MonoBehaviour
{

    public GameObject face;
    public GameObject[] allbtnColor;
    public Sprite selectedSprite, unselectedSprite;

    public void changecolor(Texture2D thisColor)
    {
        face.GetComponent<Renderer>().material.mainTexture = thisColor;
    }

    public void changeSpriteColor(Image thisImage)
    {
        for (int i = 0; i < allbtnColor.Length; i++)
        {
            allbtnColor[i].GetComponent<Image>().sprite = unselectedSprite;
           // allbtnColor[i].GetComponent<Image>().rectTransform.sizeDelta = new Vector2(56f, 52f);
        }
        thisImage.GetComponent<Image>().sprite = selectedSprite;

        //thisImage.rectTransform.sizeDelta = new Vector2(64f, 60f);
    }
}