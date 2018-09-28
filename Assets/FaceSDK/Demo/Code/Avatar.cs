using System;
using System.Collections.Generic;
using UnityEngine;
using milook;
[RequireComponent(typeof(MeshFilter))]
[RequireComponent(typeof(MeshRenderer))]
public class Avatar : MonoBehaviour
{
	internal class BlendShapeVertex
	{
		public int originalIndex;
		public Vector3 position;
		public Vector3 normal;
	}

	internal class BlendShape
	{
		public BlendShapeVertex[] vertices;
	}

	public Mesh neutral;
	public Mesh[] attributeMeshes;
	private Dictionary<int,float> attributeProgress = new Dictionary<int, float>();
	private static BlendShape[] _blendShapes;
	private Mesh _workingMesh;

	void OnTrack (MilookEngine.TrackerData tdata) {
		if (tdata.error == 0) {
			for (int i = 0; i < tdata.exp.Length; i++) {
				attributeProgress [i] = tdata.exp [i];// * attributeProgressOffset [i];
			}	
			SetMorph ();
		}
	}
	public bool showMenu;
	void OnGUI(){//return;//TODO:
		if(!showMenu) return;
		if (attributeProgress.Count <= 0) return;
		Dictionary<MilookEngine.FaceEX,string> dic = new Dictionary<MilookEngine.FaceEX, string>();
		dic[MilookEngine.FaceEX.EX_E_CLOSE_SYM] = "E_CLOSE_SYM" ;//"Symmetric eye close";
		dic[MilookEngine.FaceEX.EX_E_CLOSE_R] = "E_CLOSE_R";//"Right eye close";
		dic[MilookEngine.FaceEX.EX_E_CLOSE_L] = "E_CLOSE_L";//"Left eye close";
		dic[MilookEngine.FaceEX.EX_E_OPEN_SYM] = "OPEN_SYM";//"Symmetric wide eye open";
		dic[MilookEngine.FaceEX.EX_B_RAISE_SYM] = "B_RAISE_SYM";//"Symmetric eyebrow raise";
		dic[MilookEngine.FaceEX.EX_B_RAISE_R] = "B_RAISE_R";//"Right eyebrow raise";
		dic[MilookEngine.FaceEX.EX_B_RAISE_L] = "B_RAISE_L";//"Left eyebrow raise";


		dic [MilookEngine.FaceEX.EX_B_FURROW_SYM] = "B_FURROW_SYM";//"Symmetric eyebrow furrow";
		dic [MilookEngine.FaceEX.EX_M_AH] = "M_AH";//"Ah-shape mouth open";
		dic [MilookEngine.FaceEX.EX_M_DIS] = "M_DIS";//"Disgusted mouth shape";
		dic [MilookEngine.FaceEX.EX_M_DOWN] ="M_DOWN";//"Downward displacement of the mouth";
		dic [MilookEngine.FaceEX.EX_M_OH] =	"M_OH";//"Oh-shaped mouth";
		dic [MilookEngine.FaceEX.EX_M_EH] =	"M_EH";//"Eh-shaped mouth";
		dic [MilookEngine.FaceEX.EX_M_CLOSE_SMILE] ="CLOSE_SMILE"	;//"Mouth-closed smile";
		dic [MilookEngine.FaceEX.EX_M_OPEN_SMILE] =	"OPEN_SMILE";//"Mouth-open smile";
		dic [MilookEngine.FaceEX.EX_M_FROWN] ="M_FROWN"	;//"Frown mouth shape";
		dic [MilookEngine.FaceEX.EX_M_PULL_RIGHT] =	"PULL_RIGHT";//"Pull of the right mouth corner";
		dic [MilookEngine.FaceEX.EX_M_PULL_LEFT] ="PULL_LEFT";//"Pull of the left mouth corner";

		string obj="";
		float offsetY=38;
		foreach (KeyValuePair<MilookEngine.FaceEX,string> k in dic) {
			obj = obj + "\n"  + (int)k.Key + " : " +attributeProgress[(int)k.Key] +"   ("+k.Value+")";
			GUI.HorizontalSlider (new Rect (122, offsetY, 75, 20), attributeProgress[(int)k.Key], 0, 1);
			GUI.Label (new Rect(10, offsetY,120,30),dic[k.Key]+":");
			GUI.Label (new Rect(200, offsetY,25,30),Math.Round(attributeProgress[(int)k.Key],2).ToString());
			offsetY += 22;

		}
		//GUI.TextField (new Rect (0,200,280,300), obj);
	}
	void Start()
	{
		MeshFilter filter = (MeshFilter)gameObject.GetComponent(typeof(MeshFilter));
		filter.sharedMesh = neutral;
		_workingMesh = filter.mesh;
		if (neutral == null) return;
		int vertexCount = neutral.vertexCount;
		for (int i = 0; i < attributeMeshes.Length; i++)
		{
			if (attributeMeshes[i] == null) continue;
			if (attributeMeshes[i].vertexCount != vertexCount)
			{

				Debug.LogError("Mesh " + i + " 顶点数据量不相同 neutral vertexCount:"+vertexCount+", vCount:"+attributeMeshes[i].vertexCount);
				continue;
			}
		}

		BuildBlendShapes();
	}

	void BuildBlendShapes()
	{
		_blendShapes = new BlendShape[attributeMeshes.Length];
		for (int i = 0; i < attributeMeshes.Length; i++)
		{
			if (attributeMeshes [i] == null) continue;
			_blendShapes[i] = new BlendShape();
			Vector3[] attributeMeshVertices = attributeMeshes[i].vertices;
			Vector3[] attributeMeshNormals = attributeMeshes[i].normals;
			Vector3[] workMeshVertices = _workingMesh.vertices;
			Vector3[] workMeshNormals = _workingMesh.normals;

			List<BlendShapeVertex> vertices = new List<BlendShapeVertex>(_workingMesh.vertexCount);

			for (int j = 0; j < _workingMesh.vertexCount; j++)
			{
				if (workMeshVertices[j] != attributeMeshVertices[j])
				{
					BlendShapeVertex blendShapeVertex = new BlendShapeVertex();
					blendShapeVertex.originalIndex = j;
					blendShapeVertex.position = attributeMeshVertices[j] - workMeshVertices[j];
				//	blendShapeVertex.normal = attributeMeshNormals[j] - workMeshNormals[j];
					vertices.Add(blendShapeVertex);
				}
			}

			_blendShapes[i].vertices = vertices.ToArray();
		}
	}
	public float eOffset = 1.8f;
	public void SetMorph()
	{
		if (_blendShapes == null) return;
		Vector3[] morphedVertices = neutral.vertices;
		Vector3[] morphedNormals = neutral.normals;

		for (int j = 0; j < attributeMeshes.Length; j++)
		{
			if (attributeMeshes [j] == null) continue;
			if (!Mathf.Approximately(attributeProgress[j], 0))
			{
				BlendShape shape = _blendShapes[j];
				for (int i = 0; i < shape.vertices.Length; i++)
				{
					morphedVertices[shape.vertices[i].originalIndex] += shape.vertices[i].position * attributeProgress[j]*eOffset;
					//morphedNormals[shape.vertices[i].originalIndex] += shape.vertices[i].normal * attributeProgress[j];
				}
			}
		}

		_workingMesh.vertices = morphedVertices;
		_workingMesh.normals = morphedNormals;
		_workingMesh.RecalculateBounds();
	}

}