package by.dima;

import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.file.Paths;
import java.util.ArrayDeque;
import java.util.Arrays;
import java.util.Objects;
import java.util.concurrent.Callable;

public class MatrixMultiplier implements Callable<Matrix> {

    private static final String ARGUMENT_1 = "by.dima.Main";

    /**
     * Number of processes (queues).
     */
    private final int K;

    private final Matrix m1;

    private final Matrix m2;

    private final int resultRows;

    private final int resultCols;

    public MatrixMultiplier(Matrix m1, Matrix m2, int K)
            throws NullPointerException, IllegalArgumentException {
        Objects.requireNonNull(m1);
        Objects.requireNonNull(m2);

        if (m1.getCols() != m2.getRows()) {
            throw new IllegalArgumentException(String
                    .format("Number of columns (%d) isn't equal to number of rows (%d) of second matrix",
                            m1.getCols(), m2.getRows()));
        }

        if (K < 1) {
            throw new IllegalArgumentException(String
                    .format("Illegal number of threads: %d", K));
        }

        this.K = K;
        this.m1 = m1;
        this.m2 = m2;
        resultRows = m1.getRows();
        resultCols = m2.getCols();
    }

    @Override
    public Matrix call() throws Exception {
        Queue[] queues = splitAreaAndPutCellsToQueues();

        try (Server server = new Server()) {
            return server
                    .open()
                    .startChildProcesses()
                    .acceptAll()
                    .calculateCellsFromQueues(queues);
        }
    }

    /**
     * @return K queues with almost equal number of cells
     */
    public Queue[] splitAreaAndPutCellsToQueues() {
        Queue[] queues = new Queue[K];
        for (int i = 0; i < queues.length; i++) {
            queues[i] = new Queue();
        }

        int queueIndex = 0;

        for (int i = 0; i < resultRows; i++) {
            for (int j = 0; j < resultCols; j++) {
                Cell c = new Cell(i, j);
                queues[queueIndex].add(c);

                // go to next queue
                queueIndex++;
                queueIndex %= queues.length;
            }
        }

        return queues;
    }

    public static class Cell implements Serializable {

        /**
         * Row index.
         */
        public final int i;

        /**
         * Column index.
         */
        public final int j;

        public double result = 0;

        public Cell(int i, int j) {
            this.i = i;
            this.j = j;
        }

        @Override
        public String toString() {
            return String.format("[%d, %d]", i, j);
        }
    }

    /**
     * Is connected to K child processes through sockets,
     * and ready to send them cells.
     */
    private class Server implements AutoCloseable {

        private ServerSocket serverSocket = null;

        private final Socket[] clientSockets = new Socket[K];

        private final Process[] childProcesses = new Process[K];

        private Integer port = null;

        /**
         * Opens internal ServerSocket.
         *
         * @return this, to let chain methods
         */
        public Server open() throws IOException {
            serverSocket = new ServerSocket(0);
            port = serverSocket.getLocalPort();

            return this;
        }

        /**
         * Creates child processes.
         *
         * @throws IOException if any created processes isn't alive
         * @throws NullPointerException if server has not been opened by open() yet
         * @return this, to let chain methods
         */
        public Server startChildProcesses() throws NullPointerException, IOException {

            // create ProcessBuilder and set for him current directory
            ProcessBuilder pb = new ProcessBuilder("java", ARGUMENT_1, port.toString())
                    .directory(Paths.get("target", "classes").toFile())
                    .redirectOutput(ProcessBuilder.Redirect.INHERIT)
                    .redirectError(ProcessBuilder.Redirect.INHERIT);

            // starting child processes
            for (int i = 0; i < childProcesses.length; i++) {
                childProcesses[i] = pb.start();
            }

            // check if any process failed
            if (!Arrays.stream(childProcesses).allMatch(Process::isAlive)) {
                throw new IOException("Failed to start processes.");
            }

            return this;
        }

        /**
         * Accepts connections from child processes.
         *
         * @throws NullPointerException if server has not been opened by open() yet
         * @return this, to let chain methods
         */
        public Server acceptAll() throws NullPointerException, IOException {
            for (int i = 0; i < clientSockets.length; i++) {
                clientSockets[i] = serverSocket.accept();
            }

            return this;
        }

        /**
         * Calculates matrix multiplication.
         *
         * @param queues array of queues filled with cells
         * @throws NullPointerException if server has not been opened by open() yet,
         * child processes have not been created or accepted by server
         * @return matrix
         *
         * @implNote Sends serialized queue of sockets to corresponding socket,
         * retrieves answers and summarizes them.
         */
        public Matrix calculateCellsFromQueues(Queue[] queues)
                throws NullPointerException, IOException, ClassNotFoundException {
            double[][] mx = new double[resultRows][resultCols];

            // send cells to clients
            for (int i = 0; i < clientSockets.length; i++) {
                ObjectOutputStream oos = new ObjectOutputStream(clientSockets[i].getOutputStream());

                oos.writeObject(m1);
                oos.writeObject(m2);
                oos.writeObject(queues[i]);

                // close output stream
                clientSockets[i].shutdownOutput();
            }

            // retrieve answers
            for (Socket clientSocket : clientSockets) {
                ObjectInputStream ois = new ObjectInputStream(clientSocket.getInputStream());

                // client must calculate all cells from queue and send them back
                Queue readyCells = (Queue) ois.readObject();

                while (!readyCells.isEmpty()) {
                    Cell c = readyCells.remove();
                    mx[c.i][c.j] = c.result;
                }

                // close input stream
                clientSocket.shutdownInput();
            }

            return new Matrix(mx);
        }

        @Override
        public void close() {
            try {
                serverSocket.close();
            } catch (IOException ignore) {}

            for (Socket s : clientSockets) {
                try {
                    s.close();
                } catch (IOException ignore) {}
            }

            try {
                for (Process p : childProcesses) {
                    p.waitFor();

                    // System.out.printf(" * %s completed\n", p);
                }
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    public static class Queue extends ArrayDeque<Cell> {}
}
