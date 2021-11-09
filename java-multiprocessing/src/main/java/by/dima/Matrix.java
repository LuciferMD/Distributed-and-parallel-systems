package by.dima;

import java.io.Serializable;

import static java.util.Objects.requireNonNull;

public class Matrix implements Serializable {

    protected double[][] matrix;

    protected int rows;

    protected int cols;

    /**
     * Matrix won't be null, empty, all rows length will be equal.
     */
    public Matrix(double[][] mx) throws NullPointerException, IllegalArgumentException {
        requireNonNull(mx);
        if (mx.length == 0) {
            throw new IllegalArgumentException("No rows");
        }
        requireNonNull(mx[0]);
        if (mx[0].length == 0) {
            throw new IllegalArgumentException("No columns");
        }
        for (int i = 1; i < mx.length; i++) {
            if (mx[i - 1].length != mx[i].length) {
                throw new IllegalArgumentException(String
                        .format("Row %d length (%d) isn't equal to row %d length (%d)",
                                i - 1, mx[i - 1].length, i, mx[i].length));
            }
        }

        matrix = mx;
        rows = matrix.length;
        cols = matrix[0].length;
    }

    /**
     * @implNote A (l x m) * B (m x n) = C (l x n)
     */
    public Matrix mul(Matrix other) throws NullPointerException, IllegalArgumentException {
        requireNonNull(other);

        double[][] A = matrix;
        double[][] B = other.matrix;

        if (cols != other.rows) {
            throw new IllegalArgumentException(String
                    .format("Number of columns (%d) isn't equal to number of rows (%d) of second matrix",
                            cols, other.rows));
        }

        int l = rows;
        int m = cols;
        int n = other.cols;
        double[][] C = new double[l][n];

        for (int i = 0; i < l; i++) {
            for (int j = 0; j < n; j++) {
                for (int k = 0; k < m; k++) {
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }

        return new Matrix(C);
    }

    public double get(int i, int j) {
        return matrix[i][j];
    }

    public int getRows() {
        return rows;
    }

    public int getCols() {
        return cols;
    }
}