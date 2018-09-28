using UnityEngine;
using System.Collections;

    public class ARUtils
    {
        
        public static Vector3 GetTranslationFromMatrix (ref Matrix4x4 matrix)
        {
            Vector3 translate;
            translate.x = matrix.m03;
            translate.y = matrix.m13;
            translate.z = matrix.m23;
            return translate;
        }
        
        public static Quaternion GetRotationFromMatrix (ref Matrix4x4 matrix)
        {
            Vector3 forward;
            forward.x = matrix.m02;
            forward.y = matrix.m12;
            forward.z = matrix.m22;
            
            Vector3 upwards;
            upwards.x = matrix.m01;
            upwards.y = matrix.m11;
            upwards.z = matrix.m21;
            
            return Quaternion.LookRotation (forward, upwards);
        }
        
        public static Vector3 GetScaleFromMatrix (ref Matrix4x4 matrix)
        {
            Vector3 scale;
            scale.x = new Vector4 (matrix.m00, matrix.m10, matrix.m20, matrix.m30).magnitude;
            scale.y = new Vector4 (matrix.m01, matrix.m11, matrix.m21, matrix.m31).magnitude;
            scale.z = new Vector4 (matrix.m02, matrix.m12, matrix.m22, matrix.m32).magnitude;
            return scale;
        }
        
        public static void SetTransformFromMatrix (Transform transform, ref Matrix4x4 matrix)
        {
            transform.localPosition = GetTranslationFromMatrix (ref matrix);
            transform.localRotation = GetRotationFromMatrix (ref matrix);
            transform.localScale = GetScaleFromMatrix (ref matrix);
        }
    }
