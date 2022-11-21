using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports; // added this 

namespace ECE_323_LAB_10
{
   
    public partial class Bluetooth_Settings : Form
    {
        public  SerialPort _serial = new SerialPort(); // added this 
        public Bluetooth_Settings()
        {
            InitializeComponent();
            _serial.BaudRate = int.Parse(baud_rate.Text); // added this 
            foreach (string s in SerialPort.GetPortNames()) // added this 
            {
                com_port.Items.Add(s);
            }
        }


       
        private void Bluetooth_Settings_Load(object sender, EventArgs e)
        {

        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void connet_button_Click(object sender, EventArgs e)
        {
            try
            {
                _serial.PortName = com_port.SelectedItem.ToString();
                _serial.BaudRate = Convert.ToInt32(baud_rate.SelectedItem);
                _serial.Open();
                this.Close();
                Form1 _main = new Form1();
                foreach (Form1 tmpform in Application.OpenForms)
                {
                    if (tmpform.Name == "Form1")
                    {
                        _main = tmpform;
                        break;
                    }
                }


                _main.toolStripStatusLabel1.Text = " Connected: " + _serial.PortName.ToString();
                _main.toolStripStatusLabel1.ForeColor = Color.Green;
                _main.toolStripProgressBar1.Value = 100;
            }
            catch
            {
               MessageBox.Show("Please select COM Port/ Baud Rate");
            }            
        }
        

        private void Bluetooth_Settings_Load_1(object sender, EventArgs e)
        {
            
        }
    }
}


