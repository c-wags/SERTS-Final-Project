<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class Form1
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container()
        Me.Play = New System.Windows.Forms.Button()
        Me.Pause = New System.Windows.Forms.Button()
        Me.AudioFilePlayerGUI = New System.Windows.Forms.Label()
        Me.ProgressBar1 = New System.Windows.Forms.ProgressBar()
        Me.CurrentTime = New System.Windows.Forms.Label()
        Me.EndTime = New System.Windows.Forms.Label()
        Me.SongTitle = New System.Windows.Forms.Label()
        Me.SerialPort1 = New System.IO.Ports.SerialPort(Me.components)
        Me.IndexOfSongs = New System.Windows.Forms.ListBox()
        Me.SuspendLayout()
        '
        'Play
        '
        Me.Play.Location = New System.Drawing.Point(152, 358)
        Me.Play.Name = "Play"
        Me.Play.Size = New System.Drawing.Size(130, 54)
        Me.Play.TabIndex = 0
        Me.Play.Text = "Play"
        Me.Play.UseVisualStyleBackColor = True
        '
        'Pause
        '
        Me.Pause.Location = New System.Drawing.Point(507, 358)
        Me.Pause.Name = "Pause"
        Me.Pause.Size = New System.Drawing.Size(130, 54)
        Me.Pause.TabIndex = 1
        Me.Pause.Text = "Pause"
        Me.Pause.UseVisualStyleBackColor = True
        '
        'AudioFilePlayerGUI
        '
        Me.AudioFilePlayerGUI.AutoSize = True
        Me.AudioFilePlayerGUI.Location = New System.Drawing.Point(336, 22)
        Me.AudioFilePlayerGUI.Name = "AudioFilePlayerGUI"
        Me.AudioFilePlayerGUI.Size = New System.Drawing.Size(107, 13)
        Me.AudioFilePlayerGUI.TabIndex = 3
        Me.AudioFilePlayerGUI.Text = "Audio File Player GUI"
        '
        'ProgressBar1
        '
        Me.ProgressBar1.Location = New System.Drawing.Point(152, 322)
        Me.ProgressBar1.Name = "ProgressBar1"
        Me.ProgressBar1.Size = New System.Drawing.Size(485, 23)
        Me.ProgressBar1.TabIndex = 4
        '
        'CurrentTime
        '
        Me.CurrentTime.AutoSize = True
        Me.CurrentTime.Location = New System.Drawing.Point(119, 326)
        Me.CurrentTime.Name = "CurrentTime"
        Me.CurrentTime.Size = New System.Drawing.Size(28, 13)
        Me.CurrentTime.TabIndex = 5
        Me.CurrentTime.Text = "0:00"
        '
        'EndTime
        '
        Me.EndTime.AutoSize = True
        Me.EndTime.Location = New System.Drawing.Point(643, 326)
        Me.EndTime.Name = "EndTime"
        Me.EndTime.Size = New System.Drawing.Size(16, 13)
        Me.EndTime.TabIndex = 6
        Me.EndTime.Text = "---"
        '
        'SongTitle
        '
        Me.SongTitle.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom), System.Windows.Forms.AnchorStyles)
        Me.SongTitle.AutoSize = True
        Me.SongTitle.Location = New System.Drawing.Point(368, 299)
        Me.SongTitle.Name = "SongTitle"
        Me.SongTitle.Size = New System.Drawing.Size(52, 13)
        Me.SongTitle.TabIndex = 7
        Me.SongTitle.Text = "SongTitle"
        Me.SongTitle.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'IndexOfSongs
        '
        Me.IndexOfSongs.FormattingEnabled = True
        Me.IndexOfSongs.Location = New System.Drawing.Point(152, 50)
        Me.IndexOfSongs.Name = "IndexOfSongs"
        Me.IndexOfSongs.Size = New System.Drawing.Size(485, 238)
        Me.IndexOfSongs.TabIndex = 8
        '
        'Form1
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(787, 419)
        Me.Controls.Add(Me.IndexOfSongs)
        Me.Controls.Add(Me.SongTitle)
        Me.Controls.Add(Me.EndTime)
        Me.Controls.Add(Me.CurrentTime)
        Me.Controls.Add(Me.ProgressBar1)
        Me.Controls.Add(Me.AudioFilePlayerGUI)
        Me.Controls.Add(Me.Pause)
        Me.Controls.Add(Me.Play)
        Me.Name = "Form1"
        Me.Text = "Form1"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub

    Friend WithEvents Play As Button
    Friend WithEvents Pause As Button
    Friend WithEvents AudioFilePlayerGUI As Label
    Friend WithEvents ProgressBar1 As ProgressBar
    Friend WithEvents CurrentTime As Label
    Friend WithEvents EndTime As Label
    Friend WithEvents SongTitle As Label
    Friend WithEvents SerialPort1 As IO.Ports.SerialPort
    Friend WithEvents IndexOfSongs As ListBox
End Class
