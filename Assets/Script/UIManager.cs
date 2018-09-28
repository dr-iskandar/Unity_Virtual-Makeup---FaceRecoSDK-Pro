using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UIManager : MonoBehaviour {

    public GameObject BeautyPanel;
    public GameObject ProductsPanel;
    public GameObject ProductView;
    public GameObject Bea, Product;
    public bool panelStatus = false;

    public void ShowPanel(GameObject objek)
    {
        objek.SetActive(true);
        Bea.SetActive(false);
        Product.SetActive(false);
        panelStatus = true;
    }

    public void ClosePanel(GameObject objek)
    {
        objek.SetActive(false);
        Bea.SetActive(true);
        Product.SetActive(true);
        panelStatus = false;
    }

    public void CloseOwnPanel(GameObject objek)
    {
        objek.SetActive(false);
        ProductsPanel.SetActive(false);
        BeautyPanel.SetActive(false);
        Bea.SetActive(true);
        Product.SetActive(true);
    }

    private void Update()
    {
        if (Input.GetKey(KeyCode.Escape))
        {
            if (panelStatus == true)
            {
                if (BeautyPanel!=null)
                {
                    BeautyPanel.SetActive(false);
                }
                if (ProductsPanel!=null)
                {
                    ProductsPanel.SetActive(false);
                }
                Bea.SetActive(true);
                Product.SetActive(true);
                panelStatus = false;
            }
            else
            {
                Debug.Log("qui");
                Application.Quit();
            }
        }

    }
}