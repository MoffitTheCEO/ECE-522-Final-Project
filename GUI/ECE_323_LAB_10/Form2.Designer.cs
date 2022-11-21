
namespace ECE_323_LAB_10
{
    partial class Bluetooth_Settings
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
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
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.stop_bits = new System.Windows.Forms.ComboBox();
            this.parity = new System.Windows.Forms.ComboBox();
            this.data = new System.Windows.Forms.ComboBox();
            this.baud_rate = new System.Windows.Forms.ComboBox();
            this.com_port = new System.Windows.Forms.ComboBox();
            this.connet_button = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(7, 58);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(53, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "COM Port";
            this.label1.Click += new System.EventHandler(this.label1_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(7, 95);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(58, 13);
            this.label2.TabIndex = 1;
            this.label2.Text = "Baud Rate";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(7, 135);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(30, 13);
            this.label3.TabIndex = 2;
            this.label3.Text = "Data";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(7, 179);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(33, 13);
            this.label4.TabIndex = 3;
            this.label4.Text = "Parity";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(7, 215);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(49, 13);
            this.label5.TabIndex = 4;
            this.label5.Text = "Stop Bits";
            // 
            // stop_bits
            // 
            this.stop_bits.FormattingEnabled = true;
            this.stop_bits.Items.AddRange(new object[] {
            "1 bit ",
            "1.5 bit ",
            "2 bit "});
            this.stop_bits.Location = new System.Drawing.Point(111, 207);
            this.stop_bits.Name = "stop_bits";
            this.stop_bits.Size = new System.Drawing.Size(121, 21);
            this.stop_bits.TabIndex = 6;
            this.stop_bits.Text = "1 bit ";
            // 
            // parity
            // 
            this.parity.FormattingEnabled = true;
            this.parity.Items.AddRange(new object[] {
            "None ",
            "Odd ",
            "Even ",
            "Mark ",
            "Space"});
            this.parity.Location = new System.Drawing.Point(111, 171);
            this.parity.Name = "parity";
            this.parity.Size = new System.Drawing.Size(121, 21);
            this.parity.TabIndex = 7;
            this.parity.Text = "None";
            // 
            // data
            // 
            this.data.FormattingEnabled = true;
            this.data.Items.AddRange(new object[] {
            "8 bit ",
            "7 bit"});
            this.data.Location = new System.Drawing.Point(111, 127);
            this.data.Name = "data";
            this.data.Size = new System.Drawing.Size(121, 21);
            this.data.TabIndex = 8;
            this.data.Text = "8 bit";
            // 
            // baud_rate
            // 
            this.baud_rate.FormattingEnabled = true;
            this.baud_rate.Items.AddRange(new object[] {
            "115200",
            "9600"});
            this.baud_rate.Location = new System.Drawing.Point(111, 87);
            this.baud_rate.Name = "baud_rate";
            this.baud_rate.Size = new System.Drawing.Size(121, 21);
            this.baud_rate.TabIndex = 9;
            this.baud_rate.Text = "115200";
            // 
            // com_port
            // 
            this.com_port.FormattingEnabled = true;
            this.com_port.Location = new System.Drawing.Point(111, 50);
            this.com_port.Name = "com_port";
            this.com_port.Size = new System.Drawing.Size(121, 21);
            this.com_port.TabIndex = 10;
            // 
            // connet_button
            // 
            this.connet_button.Location = new System.Drawing.Point(111, 251);
            this.connet_button.Name = "connet_button";
            this.connet_button.Size = new System.Drawing.Size(121, 48);
            this.connet_button.TabIndex = 11;
            this.connet_button.Text = "Connect";
            this.connet_button.UseVisualStyleBackColor = true;
            this.connet_button.Click += new System.EventHandler(this.connet_button_Click);
            // 
            // Bluetooth_Settings
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(244, 323);
            this.Controls.Add(this.connet_button);
            this.Controls.Add(this.com_port);
            this.Controls.Add(this.baud_rate);
            this.Controls.Add(this.data);
            this.Controls.Add(this.parity);
            this.Controls.Add(this.stop_bits);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Name = "Bluetooth_Settings";
            this.Text = "Bluetooth Settings";
            this.Load += new System.EventHandler(this.Bluetooth_Settings_Load_1);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.ComboBox stop_bits;
        private System.Windows.Forms.ComboBox parity;
        private System.Windows.Forms.ComboBox data;
        private System.Windows.Forms.ComboBox baud_rate;
        private System.Windows.Forms.ComboBox com_port;
        private System.Windows.Forms.Button connet_button;
    }
}