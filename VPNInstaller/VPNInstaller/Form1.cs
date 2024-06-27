using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using Timer = System.Windows.Forms.Timer;

namespace VPNInstaller
{
    public partial class Form1 : Form
    {
        private Timer timer = new Timer();

        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            button2.Visible = false;
            label1.Visible = true;
            label1.Text = "Installation in progress...";

            timer.Tick += Progress;
            timer.Interval = 20;
            timer.Start();

        }

        private void Progress(object sender, EventArgs e)
        {
            if (progressBar1.Value < 100)
            {
                progressBar1.Value += 1;
            } else
            {
                timer.Stop();
                button1.Text = "Exit";
                label1.Text = "Installation completed.";
            }
        }
    }
}
