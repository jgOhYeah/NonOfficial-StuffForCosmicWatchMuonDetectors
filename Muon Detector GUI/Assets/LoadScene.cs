/* EXAMPLE OF HOW TO USE:
 * Put this script in a gameobject (main camera or similar)
 * Insert a button using Gameobject > UI > Button
 * In the OnClick() function of the button, click the + icon to add an action.
 * Drag the gameobject with this script into the gameobject field for the action.
 * Find the function GotoLevel(string sceneName) under LevelSelectorTom > GotoLevel(string sceneName).
 * This will put a text box on the action. Enter the name of the scene you want the button to call in the text box.
 * 
 * If you have errors such as scene not defined or similar, you have to add the scene in the build settings (File > Build Steetings)
*/
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class LoadScene : MonoBehaviour {
	public void GotoLevel(string sceneName){
		PlayerPrefs.Save ();
		SceneManager.LoadScene (sceneName);
	}
}
