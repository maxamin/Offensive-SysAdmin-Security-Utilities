using System.Text;

namespace Transformer
{
    public partial class Form1 : Form
    {
        private List<EncoderFile> encoders = new List<EncoderFile>();
        public Form1()
        {
            InitializeComponent();
        }

        private void findfile_Click(object sender, EventArgs e)
        {
            if(openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                string filename = openFileDialog1.FileName;
                filepath.Text = filename;
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            LoadEncodersJsonFiles();
            savetofilepath.Text = $"{Directory.GetCurrentDirectory()}\\";
        }

        private void LoadEncodersJsonFiles()
        {
            try
            {
                foreach(string path in Directory.GetFiles("configs/"))
                {
                    EncoderFile ef = Helper.ReadFile(path);
                    encoders.Add(ef);
                    options.Items.Add(ef.Name);
                }
            } catch(Exception e)
            {
                LogError($"Loading config throw an error: {e.Message}");
            }
        }

        private void LogError(string data)
        {
            log.AppendText($"{data}\r\n");
        }

        private void generate_Click(object sender, EventArgs e)
        {
            EncoderFile ef = encoders.Find(x => x.Name == options.Text);
            if(ef == null) {
                LogError("No encoder selected.");
                return;
            }

            if(String.IsNullOrEmpty(filepath.Text))
            {
                LogError("No file specified.");
                return;
            }

            if(!File.Exists(filepath.Text))
            {
                LogError($"{filepath.Text} not found.");
                return;
            }
            byte[] data = File.ReadAllBytes(filepath.Text);

            foreach(string encoder in ef.Encoders)
            {
                data = Helper.CallEncoder(encoder, data, log);
            }

            string templateData = "";
            if (!String.IsNullOrEmpty(ef.Template))
            {
                if (!File.Exists(ef.Template))
                {
                    LogError($"{ef.Template} template not found.");
                    return;
                }
                templateData = File.ReadAllText(ef.Template);
            }

            templateData = Helper.TransformVariable(ef.Pattern, templateData);
            
            if(templateData.IndexOf("{{payload}}") == -1)
            {
                templateData = Encoding.ASCII.GetString(data);
            } else
            {
                templateData = templateData.Replace("{{payload}}", Encoding.ASCII.GetString(data));
            }



            output.Text = templateData;
            LogError("Encoding process completed");
        }

        private void exit_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void savetofile_Click(object sender, EventArgs e)
        {
            if(!String.IsNullOrEmpty(output.Text))
            {
                if(!String.IsNullOrEmpty(savetofilepath.Text))
                {
                    File.WriteAllText(savetofilepath.Text, output.Text);
                    LogError($"Data saved to {savetofilepath.Text}.");
                } else
                {
                    LogError("No path specified.");
                }

            } else
            {
                LogError("There is nothing to save.");
            }
        }
    }
}