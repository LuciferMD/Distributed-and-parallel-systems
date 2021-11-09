package by.dima;

import java.io.*;
import java.net.InetAddress;
import java.net.Socket;
import java.util.Objects;
import java.util.Scanner;
import java.util.concurrent.ThreadLocalRandom;

import by.dima.MatrixMultiplier.*;

public class Main {

    public static void main(String[] args) throws Exception {
        if (args.length < 1) {
            Matrix m1 = getRandomMatrix(50, 50);
            Matrix m2 = getRandomMatrix(50, 50);
            int K;

            try (Scanner userInput = new Scanner(System.in)) {
                System.out.print("K >>> ");
                K = userInput.nextInt();
            }

            // start main thread
            MatrixMultiplier multiplier =
                    new MatrixMultiplier(m1, m2, K);
            Matrix result = multiplier.call();

            writeMatrixToFile(result, new File("ResultMatrixM.txt"));
            writeMatrixToFile(result, new File("ResultMatrixS.txt"));

            printTimeSpent(m1, m2, 1);
            printTimeSpent(m1, m2, 2);
            printTimeSpent(m1, m2, 4);
            printTimeSpent(m1, m2, 16);
        } else {

            // child process
            int port = Integer.parseInt(args[0]);
            receiveAndCalculateCells(port);
        }
    }

    private static void receiveAndCalculateCells(int port)
            throws NullPointerException, IOException, ClassNotFoundException {

        try (Socket serverSocket = new Socket(InetAddress.getLocalHost(), port);
             ObjectInputStream ois = new ObjectInputStream(serverSocket.getInputStream());
             ObjectOutputStream oos = new ObjectOutputStream(serverSocket.getOutputStream())) {

            // receive matrices queue
            Matrix m1 = (Matrix) ois.readObject();
            Matrix m2 = (Matrix) ois.readObject();
            Queue readyCells = new Queue();

            // receive cells queue
            Queue queue = (Queue) ois.readObject();

            while (!queue.isEmpty()) {
                Cell c = queue.remove();

                double sum = 0;
                for (int k = 0; k < m1.getCols(); k++) {
                    sum += m1.get(c.i, k) * m2.get(k, c.j);
                }
                c.result = sum;

                readyCells.add(c);
            }

            // send queue back
            oos.writeObject(readyCells);
        }
    }

    private static void writeMatrixToFile(Matrix m, File f)
            throws NullPointerException, IOException {
        Objects.requireNonNull(m);
        Objects.requireNonNull(f);

        try (BufferedWriter bfw = new BufferedWriter(new FileWriter(f))) {
            for (int i = 0; i < m.getRows(); i++) {
                for (int j = 0; j < m.getCols(); j++) {
                    bfw.write(String.format("  %8.3f", m.get(i, j)));
                }

                bfw.newLine();
            }
        }
    }

    private static Matrix getRandomMatrix(int rows, int cols) throws IllegalArgumentException {
        if (rows < 1 || cols < 1) {
            throw new IllegalArgumentException(String
                    .format("Illegal [rows, cols]: [%d, %d]", rows, cols));
        }

        double[][] mx = new double[rows][cols];

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                mx[i][j] = 100 * ThreadLocalRandom.current().nextDouble();
            }
        }

        return new Matrix(mx);
    }

    private static void printTimeSpent(Matrix m1, Matrix m2, int K) throws Exception {
        long begin = System.nanoTime();
        new MatrixMultiplier(m1, m2, K).call();
        long end = System.nanoTime();

        System.out.printf("%d processes: %d nanosecond\n", K, end - begin);
    }
}
