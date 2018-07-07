using UnityEngine;
using System.Collections;

public class ExitProgram : MonoBehaviour {
	public void Exit() {
		Application.Quit ();
	}
	void Update () {
		if (Input.GetKeyDown("escape")) {
			Application.Quit ();
		}
	}
}