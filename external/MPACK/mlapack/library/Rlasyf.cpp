/*
 * Copyright (c) 2008-2010
 *      Nakata, Maho
 *      All rights reserved.
 *
 *  $Id: Rlasyf.cpp,v 1.11 2010/08/07 04:48:33 nakatamaho Exp $ 
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */
/*
Copyright (c) 1992-2007 The University of Tennessee.  All rights reserved.

$COPYRIGHT$

Additional copyrights may follow

$HEADER$

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

- Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer. 
  
- Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer listed
  in this license in the documentation and/or other materials
  provided with the distribution.
  
- Neither the name of the copyright holders nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.
  
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT  
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT  
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
*/

#include <mblas.h>
#include <mlapack.h>

void Rlasyf(const char *uplo, INTEGER n, INTEGER nb, INTEGER kb, REAL * A, INTEGER lda, INTEGER * ipiv, REAL * w, INTEGER ldw, INTEGER * info)
{
    INTEGER imax = 0, j, jb, jj, jmax, jp, k, kk, kkw, kp;
    INTEGER kstep, kw;
    REAL absakk, alpha, colmax, d11, d21, d22;
    REAL rowmax, t, r1;
    REAL Zero = 0.0, One = 1.0, Eight = 8.0, Seventeen = 17.0;
    REAL mtemp1, mtemp2;

    *info = 0;
//Initialize ALPHA for use in choosing pivot block size.
    alpha = (One + sqrt(Seventeen)) / Eight;
    if (Mlsame(uplo, "U")) {
//Factorize the trailing columns of A using the upper triangle
//of A and working backwards, and compute the matrix W = U12*D
//for use in updating A11
//K is the main loop index, decreasing from N in steps of 1 or 2
//KW is the column of W which corresponds to column K of A
	k = n;
	while (1) {
	    kw = nb + k - n;
//Exit from loop
	    if ((k <= n - nb + 1 && nb < n) || k < 1)
		break;
//Copy column K of A to column KW of W and update it
	    Rcopy(k, &A[(k - 1) * lda], 1, &w[(kw - 1) * ldw], 1);
	    if (k < n) {
		Rgemv("No transpose", k, n - k, -One, &A[k * lda], lda, &w[(k - 1) + kw * ldw], ldw, One, &w[(kw - 1) * ldw], 1);
	    }
	    kstep = 1;
//Determine rows and columns to be interchanged and whether
//a 1-by-1 or 2-by-2 pivot block will be used
	    absakk = abs(w[(k - 1) + (kw - 1) * ldw]);
//IMAX is the row-index of the largest off-diagonal element in
//column K, and COLMAX is its absolute value
	    if (k > 1) {
		imax = iRamax(k - 1, &w[(kw - 1) * ldw], 1);
		colmax = abs(w[(imax - 1) + (kw - 1) * ldw]);
	    } else {
		colmax = Zero;
	    }
	    if (max(absakk, colmax) == Zero) {
//Column K is zero: set INFO and continue
		if (*info == 0) {
		    *info = k;
		}
		kp = k;
	    } else {
		if (absakk >= alpha * colmax) {
//no interchange, use 1-by-1 pivot block
		    kp = k;
		} else {
//Copy column IMAX to column KW-1 of W and update it
		    Rcopy(imax, &A[(imax - 1) * lda], 1, &w[(kw - 2) * ldw], 1);
		    Rcopy(k - imax, &A[(imax - 1) + imax * lda], lda, &w[imax + (kw - 2) * ldw], 1);
		    if (k < n) {
			Rgemv("No transpose", k, n - k, -One, &A[k * lda], lda, &w[(imax - 1) + kw * ldw], ldw, One, &w[(kw - 2) * ldw], 1);
		    }
//JMAX is the column-index of the largest off-diagonal
//element in row IMAX, and ROWMAX is its absolute value
		    jmax = imax + iRamax(k - imax, &w[imax + (kw - 2) * ldw], 1);
		    rowmax = abs(w[(jmax - 1) + (kw - 2) * ldw]);
		    if (imax > 1) {
			jmax = iRamax(imax - 1, &w[(kw - 2) * ldw], 1);
			mtemp1 = rowmax, mtemp2 = abs(w[(jmax - 1) + (kw - 2) * ldw]);
			rowmax = max(mtemp1, mtemp2);
		    }
		    if (absakk >= alpha * colmax * (colmax / rowmax)) {
//no interchange, use 1-by-1 pivot block
			kp = k;
		    } else if (abs(w[(imax - 1) + (kw - 2) * ldw]) >= alpha * rowmax) {
//interchange rows and columns K and IMAX, use 1-by-1
//pivot block
			kp = imax;
//copy column KW-1 of W to column KW
			Rcopy(k, &w[(kw - 2) * ldw], 1, &w[(kw - 1) * ldw], 1);
		    } else {
//interchange rows and columns K-1 and IMAX, use 2-by-2
//pivot block
			kp = imax;
			kstep = 2;
		    }
		}
		kk = k - kstep + 1;
		kkw = nb + kk - n;
//Updated column KP is already stored in column KKW of W
		if (kp != kk) {
//Copy non-updated column KK to column KP
		    A[(kp - 1) + (k - 1) * lda] = A[(kk - 1) + (k - 1) * lda];
		    Rcopy(k - 1 - kp, &A[kp + (kk - 1) * lda], 1, &A[(kp - 1) + kp * lda], lda);
		    Rcopy(kp, &A[(kk - 1) * lda], 1, &A[(kp - 1) * lda], 1);
//Interchange rows KK and KP in last KK columns of A and W
		    Rswap(n - kk + 1, &A[(kk - 1) + (kk - 1) * lda], lda, &A[(kp - 1) + (kk - 1) * lda], lda);
		    Rswap(n - kk + 1, &w[(kk - 1) + (kkw - 1) * ldw], ldw, &w[(kp - 1) + (kkw - 1) * ldw], ldw);
		}
		if (kstep == 1) {
//1-by-1 pivot block D(k): column KW of W now holds
//W(k) = U(k)*D(k)
//where U(k) is the k-th column of U
//Store U(k) in column k of A
		    Rcopy(k, &w[(kw - 1) * ldw], 1, &A[(k - 1) * lda], 1);
		    r1 = One / A[(k - 1) + (k - 1) * lda];
		    Rscal(k - 1, r1, &A[(k - 1) * lda], 1);
		} else {
//2-by-2 pivot block D(k): columns KW and KW-1 of W now
//hold
//( W(k-1) W(k) ) = ( U(k-1) U(k) )*D(k)
//where U(k) and U(k-1) are the k-th and (k-1)-th columns
//of U
		    if (k > 2) {
//Store U(k) and U(k-1) in columns k and k-1 of A
			d21 = w[k - 2 + (kw - 1) * ldw];
			d11 = w[k - 1 + (kw - 1) * ldw] / d21;
			d22 = w[k - 2 + (kw - 2) * ldw] / d21;
			t = One / (d11 * d22 - One);
			d21 = t / d21;
			for (j = 1; j <= k - 2; j++) {
			    A[(j - 1) + (k - 2) * lda] = d21 * (d11 * w[(j - 1) + (kw - 2) * ldw] - w[(j - 1) + (kw - 1) * ldw]);
			    A[(j - 1) + (k - 1) * lda] = d21 * (d22 * w[(j - 1) + (kw - 1) * ldw] - w[(j - 1) + (kw - 2) * ldw]);
			}
		    }
//Copy D(k) to A
		    A[k - 2 + (k - 2) * lda] = w[k - 2 + (kw - 2) * ldw];
		    A[k - 2 + (k - 1) * lda] = w[k - 2 + (kw - 1) * ldw];
		    A[k - 1 + (k - 1) * lda] = w[k - 1 + (kw - 1) * ldw];
		}
	    }
//Store details of the interchanges in IPIV
	    if (kstep == 1) {
		ipiv[k - 1] = kp;
	    } else {
		ipiv[k - 1] = -kp;
		ipiv[k - 2] = -kp;
	    }
//Decrease K and return to the start of the main loop
	    k = k - kstep;
	}
//Update the upper triangle of A11 (= A(1:k,1:k)) as
//A11 := A11 - U12*D*U12' = A11 - U12*W'
//computing blocks of NB columns at a time
	for (j = (k - 1) / nb * nb + 1; j >= 1; j = j - nb) {
	    jb = min(nb, k - j + 1);
//Update the upper triangle of the diagonal block
	    for (jj = j; jj <= j + jb - 1; jj++) {
		Rgemv("No transpose", jj - j + 1, n - k, -One, &A[(j - 1) + k * lda], lda, &w[(jj - 1) + kw * ldw], ldw, One, &A[(j - 1) + (jj - 1) * lda], 1);
	    }
//Update the rectangular superdiagonal block
	    Rgemm("No transpose", "Transpose", j - 1, jb, n - k, -One, &A[k * lda], lda, &w[(j - 1) + kw * ldw], ldw, One, &A[(j - 1) * lda], lda);
	}
//Put U12 in standard form by partially undoing the interchanges
//in columns k+1:n
	j = k + 1;
	while (1) {
	    jj = j;
	    jp = ipiv[j - 1];
	    if (jp < 0) {
		jp = -jp;
		j++;
	    }
	    j++;
	    if (jp != jj && j <= n) {
		Rswap(n - j + 1, &A[(jp - 1) + (j - 1) * lda], lda, &A[(jj - 1) + (j - 1) * lda], lda);
	    }
	    if (j > n)
		break;
	}
//Set KB to the number of columns factorized
	kb = n - k;
    } else {
//Factorize the leading columns of A using the lower triangle
//of A and working forwards, and compute the matrix W = L21*D
//for use in updating A22
//K is the main loop index, increasing from 1 in steps of 1 or 2
	k = 1;
	while (1) {
//Exit from loop
	    if ((k >= nb && nb < n) || k > n)
		break;
//Copy column K of A to column K of W and update it
	    Rcopy(n - k + 1, &A[(k - 1) + (k - 1) * lda], 1, &w[(k - 1) + (k - 1) * ldw], 1);
	    Rgemv("No transpose", n - k + 1, k - 1, -One, &A[k - 1], lda, &w[k - 1], ldw, One, &w[(k - 1) + (k - 1) * ldw], 1);
	    kstep = 1;
//Determine rows and columns to be interchanged and whether
//a 1-by-1 or 2-by-2 pivot block will be used
	    absakk = abs(w[(k - 1) + (k - 1) * ldw]);
//IMAX is the row-index of the largest off-diagonal element in
//column K, and COLMAX is its absolute value
	    if (k < n) {
		imax = k + iRamax(n - k, &w[k + (k - 1) * ldw], 1);
		colmax = abs(w[(imax - 1) + (k - 1) * ldw]);
	    } else {
		colmax = Zero;
	    }
	    if (max(absakk, colmax) == Zero) {
//Column K is zero: set INFO and continue
		if (*info == 0) {
		    *info = k;
		}
		kp = k;
	    } else {
		if (absakk >= alpha * colmax) {
//no interchange, use 1-by-1 pivot block
		    kp = k;
		} else {
//Copy column IMAX to column K+1 of W and update it
		    Rcopy(imax - k, &A[(imax - 1) + (k - 1) * lda], lda, &w[(k - 1) + k * ldw], 1);
		    Rcopy(n - imax + 1, &A[(imax - 1) + (imax - 1) * lda], 1, &w[(imax - 1) + k * ldw], 1);
		    Rgemv("No transpose", n - k + 1, k - 1, -One, &A[k - 1], lda, &w[imax - 1], ldw, One, &w[(k - 1) + k * ldw], 1);
//JMAX is the column-index of the largest off-diagonal
//element in row IMAX, and ROWMAX is its absolute value
		    jmax = k - 1 + iRamax(imax - k, &w[(k - 1) + k * ldw], 1);
		    rowmax = abs(w[(jmax - 1) + k * ldw]);
		    if (imax < n) {
			jmax = imax + iRamax(n - imax, &w[imax + k * ldw], 1);
			mtemp1 = rowmax, mtemp2 = abs(w[(jmax - 1) + k * ldw]);
			rowmax = max(mtemp1, mtemp2);
		    }
		    if (absakk >= alpha * colmax * (colmax / rowmax)) {
//no interchange, use 1-by-1 pivot block
			kp = k;
		    } else if (abs(w[imax - 1 + k * ldw]) >= alpha * rowmax) {
//interchange rows and columns K and IMAX, use 1-by-1
//pivot block
			kp = imax;
//copy column K+1 of W to column K
			Rcopy(n - k + 1, &w[k - 1 + k * ldw], 1, &w[(k - 1) + (k - 1) * ldw], 1);
		    } else {
//interchange rows and columns K+1 and IMAX, use 2-by-2
//pivot block
			kp = imax;
			kstep = 2;
		    }
		}
		kk = k + kstep - 1;
//Updated column KP is already stored in column KK of W
		if (kp != kk) {
//Copy non-updated column KK to column KP
		    A[(kp - 1) + (k - 1) * lda] = A[(kk - 1) + (k - 1) * lda];
		    Rcopy(kp - k - 1, &A[k + (kk - 1) * lda], 1, &A[(kp - 1) + k * lda], lda);
		    Rcopy(n - kp + 1, &A[(kp - 1) + (kk - 1) * lda], 1, &A[(kp - 1) + (kp - 1) * lda], 1);
//Interchange rows KK and KP in first KK columns of A and W
		    Rswap(kk, &A[kk - 1], lda, &A[kp - 1], lda);
		    Rswap(kk, &w[kk - 1], ldw, &w[kp - 1], ldw);
		}
		if (kstep == 1) {
//1-by-1 pivot block D(k): column k of W now holds
//W(k) = L(k)*D(k)
//where L(k) is the k-th column of L
//Store L(k) in column k of A
		    Rcopy(n - k + 1, &w[(k - 1) + (k - 1) * ldw], 1, &A[(k - 1) + (k - 1) * lda], 1);
		    if (k < n) {
			r1 = One / A[(k - 1) + (k - 1) * lda];
			Rscal(n - k, r1, &A[k + (k - 1) * lda], 1);
		    }
		} else {
//2-by-2 pivot block D(k): columns k and k+1 of W now hold
//( W(k) W(k+1) ) = ( L(k) L(k+1) )*D(k)
//where L(k) and L(k+1) are the k-th and (k+1)-th columns
//of L
		    if (k < n - 1) {
//Store L(k) and L(k+1) in columns k and k+1 of A
			d21 = w[k + (k - 1) * ldw];
			d11 = w[k + k * ldw] / d21;
			d22 = w[(k - 1) + (k - 1) * ldw] / d21;
			t = One / (d11 * d22 - One);
			d21 = t / d21;
			for (j = k + 2; j <= n; j++) {
			    A[(j - 1) + (k - 1) * lda] = d21 * (d11 * w[(j - 1) + (k - 1) * ldw] - w[(j - 1) + k * ldw]);
			    A[(j - 1) + k * lda] = d21 * (d22 * w[(j - 1) + k * ldw] - w[(j - 1) + (k - 1) * ldw]);
			}
		    }
//Copy D(k) to A
		    A[(k - 1) + (k - 1) * lda] = w[(k - 1) + (k - 1) * ldw];
		    A[k + (k - 1) * lda] = w[k + (k - 1) * ldw];
		    A[k + k * lda] = w[k + k * ldw];
		}
	    }
//Store details of the interchanges in IPIV
	    if (kstep == 1) {
		ipiv[k - 1] = kp;
	    } else {
		ipiv[k - 1] = -kp;
		ipiv[k] = -kp;
	    }
//Increase K and return to the start of the main loop
	    k = k + kstep;
	}
//Update the lower triangle of A22 (= A(k:n,k:n)) as
//A22 := A22 - L21*D*L21' = A22 - L21*W'
//computing blocks of NB columns at a time
	for (j = k; j <= n; j = j + nb) {
	    jb = min(nb, n - j + 1);
//Update the lower triangle of the diagonal block
	    for (jj = j; jj <= j + jb - 1; jj++) {
		Rgemv("No transpose", j + jb - jj, k - 1, -One, &A[jj - 1], lda, &w[jj - 1], ldw, One, &A[jj - 1 + (jj - 1) * lda], 1);
	    }
//Update the rectangular subdiagonal block
	    if (j + jb <= n) {
		Rgemm("No transpose", "Transpose", n - j - jb + 1, jb, k - 1, -One, &A[j + jb - 1], lda, &w[j - 1], ldw, One, &A[(j + jb - 1) + (j - 1) * lda], lda);
	    }
	}
//Put L21 in standard form by partially undoing the interchanges
//in columns 1:k-1
	j = k - 1;
	while (1) {
	    jj = j;
	    jp = ipiv[j - 1];
	    if (jp < 0) {
		jp = -jp;
		j--;
	    }
	    j--;
	    if (jp != jj && j >= 1) {
		Rswap(j, &A[jp - 1], lda, &A[jj - 1], lda);
	    }
	    if (j < 1)
		break;
	}
//Set KB to the number of columns factorized
	kb = k - 1;
    }
    return;
}
