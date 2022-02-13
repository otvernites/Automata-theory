package ru.mephi.laboratory1v3;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;


public class Main {

    public static void main(String[] args) throws IOException {
        long startTime = System.nanoTime();

        FileReader fileReader = new FileReader("C:\\Users\\Asus\\Desktop\\Теория автоматов\\Laboratory1\\V3" +
                "\\src\\main\\java\\ru\\mephi\\laboratory1v3\\100000.txt");
        BufferedReader reader = new BufferedReader(fileReader);

        long nanoseconds = 0;

        MyLexer ftp = new MyLexer(reader);
        ftp.yylex();
        long endTime = System.nanoTime();
        nanoseconds += endTime - startTime;
/*
        System.out.println("Correct lines:");
        for (MyLexer.Pair el : ftp.results) {
            el.printPair();
        }

        System.out.println("\nFile name - Number of repetitions");
        for (MyLexer.Pair el : ftp.nameStat) {
            el.printPair();
        }
*/
        System.out.println(nanoseconds);
    };
}
