Imports System.Threading

Public Class Form1

    'Definitions
    Delegate Sub TextBoxDelegate(ByVal myStr As String)
    Dim Thread_0 As New Thread(AddressOf Thread_0_method) 'Initialization Thread
    Dim Thread_1 As New Thread(AddressOf Thread_1_method) 'Play Thread
    Dim Thread_2 As New Thread(AddressOf Thread_2_method) 'Pause Thread
    Delegate Sub ListBoxDelegate(ByVal theString As String)
    Dim ListBoxDel As New ListBoxDelegate(AddressOf ListBoxDelMethod)

    Private Sub Pause_Click(sender As Object, e As EventArgs) Handles Pause.Click
        'When button is clicked send an R to get the red response
        If SerialPort1.IsOpen Then
            SerialPort1.Write("I", 0, 1) 'Send the pause command to the AFP
            SerialPort1.Write(vbNullChar, 0, 1)
        End If
    End Sub

    Private Sub Play_Click(sender As Object, e As EventArgs) Handles Play.Click
        'When button is clicked send a G to get the green response
        If SerialPort1.IsOpen Then
            SerialPort1.Write("P", 0, 1) 'Send the play command to the AFP
            SerialPort1.Write(vbNullChar, 0, 1)
        End If

    End Sub

    Private Sub Form1_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        'Trying the connection to the Serial Port
        Try
            SerialPort1.Open()
        Catch
            Console.WriteLine("Failed to open serial port")
        End Try
        'Thread_0.Start()
    End Sub
    'Initialization Thread
    Private Sub Thread_0_method()
        'SerialPort1.ReadTimeout = 1000
        SerialPort1.Write("N", 0, 1) 'Send initialization command to the AFP
        SerialPort1.Write(vbNullChar, 0, 1)
        'Dim str As String
        Dim initString As String
        'Str = "Init"
        'While (str = "Init")
        While 1
            If SerialPort1.IsOpen Then
                'Getting the file names that the AFP sends during initialization
                'initString = SerialPort1.ReadLine()
                While SerialPort1.BytesToRead > 0
                    'SerialPort1.Read(initString, 0, 50)
                    initString = SerialPort1.ReadLine()
                    'System.Diagnostics.Debug.Write(initString)
                    'If (initString = "Done_Init") Then
                    ' Str = "Done_Init"
                    'Else
                    'IndexOfSongs.Invoke(ListBoxDel, initString)
                    'End If
                    IndexOfSongs.Invoke(ListBoxDel, initString)
                End While
            End If
        End While
        ' Sort songs alphabetically
        IndexOfSongs.Sorted = True
        GetSongListButton.Enabled = False
    End Sub
    'The Play Thread
    Private Sub Thread_1_method()
        Dim str As String
        While SerialPort1.BytesToRead < 0

        End While
    End Sub
    'The Pause Thread
    Private Sub Thread_2_method()
        Dim str As String
        While SerialPort1.BytesToRead < 0

        End While
    End Sub

    Private Sub GetSongListButton_Click(sender As Object, e As EventArgs) Handles GetSongListButton.Click
        Thread_0.Start()
    End Sub

    Private Sub ListBoxDelMethod(ByVal theString As String)
        IndexOfSongs.Items.Add(theString)
    End Sub
End Class