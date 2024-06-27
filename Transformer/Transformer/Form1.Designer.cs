namespace Transformer
{
    partial class Form1
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.output = new System.Windows.Forms.TextBox();
            this.log = new System.Windows.Forms.TextBox();
            this.filepath = new System.Windows.Forms.TextBox();
            this.findfile = new System.Windows.Forms.Button();
            this.options = new System.Windows.Forms.ComboBox();
            this.generate = new System.Windows.Forms.Button();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.exit = new System.Windows.Forms.Button();
            this.savetofile = new System.Windows.Forms.Button();
            this.savetofilepath = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // output
            // 
            this.output.Location = new System.Drawing.Point(472, 12);
            this.output.Multiline = true;
            this.output.Name = "output";
            this.output.Size = new System.Drawing.Size(450, 549);
            this.output.TabIndex = 0;
            this.output.WordWrap = false;
            // 
            // log
            // 
            this.log.Location = new System.Drawing.Point(12, 12);
            this.log.Multiline = true;
            this.log.Name = "log";
            this.log.ReadOnly = true;
            this.log.Size = new System.Drawing.Size(454, 282);
            this.log.TabIndex = 1;
            // 
            // filepath
            // 
            this.filepath.Location = new System.Drawing.Point(12, 300);
            this.filepath.Name = "filepath";
            this.filepath.Size = new System.Drawing.Size(351, 23);
            this.filepath.TabIndex = 2;
            // 
            // findfile
            // 
            this.findfile.Location = new System.Drawing.Point(369, 299);
            this.findfile.Name = "findfile";
            this.findfile.Size = new System.Drawing.Size(97, 23);
            this.findfile.TabIndex = 3;
            this.findfile.Text = "Browse...";
            this.findfile.UseVisualStyleBackColor = true;
            this.findfile.Click += new System.EventHandler(this.findfile_Click);
            // 
            // options
            // 
            this.options.FormattingEnabled = true;
            this.options.Location = new System.Drawing.Point(12, 329);
            this.options.Name = "options";
            this.options.Size = new System.Drawing.Size(454, 23);
            this.options.TabIndex = 4;
            // 
            // generate
            // 
            this.generate.Location = new System.Drawing.Point(12, 538);
            this.generate.Name = "generate";
            this.generate.Size = new System.Drawing.Size(75, 23);
            this.generate.TabIndex = 5;
            this.generate.Text = "Generate";
            this.generate.UseVisualStyleBackColor = true;
            this.generate.Click += new System.EventHandler(this.generate_Click);
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileName = "openFileDialog1";
            // 
            // exit
            // 
            this.exit.Location = new System.Drawing.Point(369, 538);
            this.exit.Name = "exit";
            this.exit.Size = new System.Drawing.Size(97, 23);
            this.exit.TabIndex = 6;
            this.exit.Text = "Exit";
            this.exit.UseVisualStyleBackColor = true;
            this.exit.Click += new System.EventHandler(this.exit_Click);
            // 
            // savetofile
            // 
            this.savetofile.Location = new System.Drawing.Point(369, 417);
            this.savetofile.Name = "savetofile";
            this.savetofile.Size = new System.Drawing.Size(97, 23);
            this.savetofile.TabIndex = 7;
            this.savetofile.Text = "Save To File";
            this.savetofile.UseVisualStyleBackColor = true;
            this.savetofile.Click += new System.EventHandler(this.savetofile_Click);
            // 
            // savetofilepath
            // 
            this.savetofilepath.Location = new System.Drawing.Point(12, 417);
            this.savetofilepath.Name = "savetofilepath";
            this.savetofilepath.Size = new System.Drawing.Size(351, 23);
            this.savetofilepath.TabIndex = 8;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(934, 573);
            this.Controls.Add(this.savetofilepath);
            this.Controls.Add(this.savetofile);
            this.Controls.Add(this.exit);
            this.Controls.Add(this.generate);
            this.Controls.Add(this.options);
            this.Controls.Add(this.findfile);
            this.Controls.Add(this.filepath);
            this.Controls.Add(this.log);
            this.Controls.Add(this.output);
            this.Name = "Form1";
            this.Text = "Transformer";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private TextBox output;
        private TextBox log;
        private TextBox filepath;
        private Button findfile;
        private ComboBox options;
        private Button generate;
        private OpenFileDialog openFileDialog1;
        private Button exit;
        private Button savetofile;
        private TextBox savetofilepath;
    }
}