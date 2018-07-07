using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.IO.Ports;
using System.Text;
using UnityEngine;
using UnityEngine.UI;


public class serialComs : MonoBehaviour {
    public Dropdown setPortsDropdown;
    public Text portsErrorMsg;
    public Button refreshButton;

    public Text displayCount;
    public Text temperatureText;
    public Text upTimeText;
    public Text rateText;
    public Text sipmText;
    public Text deadTimeText;
    public Text terminalText;
    private int maxInput = 2;
    public float graphUpdateSpeed = 2f;
    public DD_DataDiagram countRateGraph;
    public DD_DataDiagram sipmGraph;
    private string[] serialPortList;
    private SerialPort stream;
    private bool readSerial = true;

    private long totalCount = 0;
    private long timeStamp = 0;
    private int measuredADC = 0;
    private float sipmVolts = 0f;
    private long deadTime = 0;
    private float temperature = 0f;
    private float timeDiff = 0f;

    private bool isFirstTime = true;
    private GameObject sipmGraphLine;
    private GameObject countRateGraphLine;

    private float firstTime;
    private long firstCount;

    private float lastScreenHeight;
    List<string> input = new List<string>();
    public IEnumerator AsynchronousReadFromArduino()
    {
        while (readSerial)
        {
            if (stream != null)
            {
                if (stream.IsOpen)
                {
                    try
                    {
                        string data = stream.ReadLine();
                        print(data);
                        addText(data);
                        string[] dataSplit = data.Split(new char[] { ' ', ',' }, StringSplitOptions.RemoveEmptyEntries);
                        //string[] dataSplit = data.Split(',');
                        //Must have 6 parts to be counted as a row of data
                        if (dataSplit.Length == 6)
                        {
                            try
                            {
                                totalCount = long.Parse(dataSplit[0]);
                                timeStamp = long.Parse(dataSplit[1]);
                                measuredADC = int.Parse(dataSplit[2]);
                                sipmVolts = float.Parse(dataSplit[3]);
                                deadTime = long.Parse(dataSplit[4]);
                                temperature = float.Parse(dataSplit[5]);
                                print("Count: " + totalCount + ", Time Stamp: " + timeStamp + ", Measured ADC: " + measuredADC + ", SiPM Volts: " + sipmVolts + ", Dead time: " + deadTime + ", Temperature: " + temperature);
                                displayCount.text = dataSplit[0];
                                temperatureText.text = string.Format("Temperture: {0:#0.0}°C", temperature);
                                sipmText.text = string.Format("SiPM voltage estimate: {0:#0.00}mV", sipmVolts);
                                //If the arduino has been reset, restart the timer
                                if (totalCount == 1)
                                {
                                    isFirstTime = true;
                                }
                                //Calculate the difference in time between the application's timestamp and the arduino's time stamp.
                                //Because we trust thecomputer's clock more than the arduino's, we only calculate the difference once and use this to work out what the actual time should be.
                                if (isFirstTime)
                                {
                                    timeDiff = ((float)timeStamp / 1000f) - Time.time;
                                    isFirstTime = false;
                                    portsErrorMsg.text = "The connection has experienced success";
                                }
                                //float runningTime = Time.realtimeSinceStartup + timeDiff;
                                sipmGraph.InputPoint(sipmGraphLine, new Vector2(0.1f, sipmVolts));
                                deadTimeText.text = string.Format("Dead time: {0}%", 100 * deadTime / timeStamp);
                            }
                            catch (FormatException)
                            {
                                print("Could not format as a number");
                            }
                        }
                    }
                    catch (TimeoutException)
                    {
                    }
                }
            }
            yield return new WaitForSeconds(0.25f);
        }
    }
    public IEnumerator updateCountGraph()
    {
        while (true)
        {

            float currentTime = Time.time + timeDiff;
            if (totalCount > 0)
            {
                float rate = (totalCount - firstCount) / (currentTime - firstTime);
                countRateGraph.InputPoint(countRateGraphLine, new Vector2(0.1f, rate));
            }
            firstCount = totalCount;
            firstTime = currentTime;
            yield return new WaitForSeconds(graphUpdateSpeed);
        }
    }
    // Use this for initialization
    void Start() {
        Application.targetFrameRate = 15; //Slow down the frame rate to save power as not doing anything crazy with graphics.
        sipmGraphLine = sipmGraph.AddLine("SiPM Voltage", Color.red);
        countRateGraphLine = countRateGraph.AddLine("Count Rate", Color.green);
        getPorts();
        setPortsDropdown.onValueChanged.AddListener(delegate { openPort(); });
        refreshButton.onClick.AddListener(delegate { getPorts(); });
        StartCoroutine(AsynchronousReadFromArduino());
        StartCoroutine(updateCountGraph());
        maxInput = GetMaxLineCount(terminalText);
        lastScreenHeight = Screen.height;
    }

    // Update is called once per frame
    void Update()
    {
        //Work out the up time of the detector. The computer syncs to the detector's clock. Probably not the most accurate, but does not really matter as would would be using serial anyway if accuracy is required.
        float runningTime = Time.time + timeDiff;
        int hours = Mathf.FloorToInt(runningTime / 3600F);
        int minutes = Mathf.FloorToInt((runningTime - hours * 3600) / 60F);
        int seconds = Mathf.FloorToInt(runningTime - hours * 3600 - minutes * 60);
        upTimeText.text = string.Format("Up time: {0}:{1:#00}:{2:#00}", hours, minutes, seconds);

        float rate = 0;
        float rateStd = 0;
        float detectionTime = runningTime - (deadTime / 1000f);
        if (totalCount > 0)
        {
            rate = totalCount / detectionTime;
            rateStd = Mathf.Sqrt((float)totalCount) / detectionTime;
        }
        rateText.text = string.Format("Rate: {0:#0.00} ±{1:#0.000} counts per second", rate, rateStd);
        if(Screen.height != lastScreenHeight)
        {
            lastScreenHeight = Screen.height;
            maxInput = GetMaxLineCount(terminalText);
            print("Screen Resized");
            while (input.Count >= maxInput && maxInput > -1)
            {
                input.RemoveAt(0);
            }
            printInput();
        }
    }
    void openPort()
    {
        if (stream != null)
        {
            stream.Close();
        }
        int selectedOption = setPortsDropdown.value;
        stream = new SerialPort(serialPortList[selectedOption], 9600);
        stream.ReadTimeout = 50;
        try
        {
            stream.Open();
        } catch (IOException)
        {

        }
        string errorMsg;
        if (!stream.IsOpen)
        {
            errorMsg = "Could not open port " + serialPortList[selectedOption].ToString() + ". Is another program using it?";
        } else
        {
            errorMsg = "Port " + serialPortList[selectedOption].ToString() + " successfully opened.";
        }
        Debug.Log(errorMsg);
        portsErrorMsg.text = errorMsg;
    }
    void getPorts()
    {
        setPortsDropdown.ClearOptions();
        serialPortList = SerialPort.GetPortNames();
        List<string> portsList;
        //Only try to open a port if there is at least one available
        if (serialPortList.Length > 0)
        {
            portsList = new List<string>(serialPortList);
            openPort();
        } else
        {
            portsList = new List<string> { "Nothing Connected" };
        }
        setPortsDropdown.AddOptions(portsList);
    }
    void addText(string newInput)
    {
        if (maxInput != 0)
        {
            if (input.Count == maxInput)
            {
                //input.RemoveAt(input.Count - 1);
                input.RemoveAt(0);
            }
            input.Add(newInput);
            printInput();
        }
    }
    void printInput()
    {
        terminalText.text = ""; //Clear the text box
        foreach (string str in input)
        {
            terminalText.text += str;
            terminalText.text += "\r\n";
        }
    }
    public static int GetMaxLineCount(Text text)
    {
        var textGenerator = new TextGenerator();
        var generationSettings = text.GetGenerationSettings(text.rectTransform.rect.size);
        var lineCount = 0;
        var s = new StringBuilder();
        while (true)
        {
            s.Append("\n");
            textGenerator.Populate(s.ToString(), generationSettings);
            var nextLineCount = textGenerator.lineCount;
            if (lineCount == nextLineCount) break;
            lineCount = nextLineCount;
        }
        return lineCount;
    }
}