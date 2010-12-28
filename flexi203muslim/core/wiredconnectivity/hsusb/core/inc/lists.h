/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 












This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.











************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
/*
 * Copyright (c) 2004 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Lennart Augustsson (lennart@augustsson.net) and by Charles M. Hannum.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _LISTS_H_
#define _LISTS_H_

#ifndef _SYS_QUEUE_H_

#define QMD_TRACE_ELEM(elem)
#define QMD_TRACE_HEAD(head)
#define TRASHIT(x)
#define TRACEBUF
#define TAILQ_ENTRY(type)                                               \
struct {                                                                \
        struct type *tqe_next;  /* next element */                      \
        struct type **tqe_prev; /* address of previous next element */  \
        TRACEBUF                                                        \
}
#define TAILQ_HEAD(name, type)                                          \
struct name {                                                           \
        struct type *tqh_first; /* first element */                     \
        struct type **tqh_last; /* addr of last next element */         \
        TRACEBUF                                                        \
}
#define TAILQ_FIRST(head)       ((head)->tqh_first)
#define TAILQ_NEXT(elm, field) ((elm)->field.tqe_next)
#define TAILQ_REMOVE(head, elm, field) do {                             \
        if ((TAILQ_NEXT((elm), field)) != NULL)                         \
                TAILQ_NEXT((elm), field)->field.tqe_prev =              \
                    (elm)->field.tqe_prev;                              \
        else {                                                          \
                (head)->tqh_last = (elm)->field.tqe_prev;               \
                QMD_TRACE_HEAD(head);                                   \
        }                                                               \
        *(elm)->field.tqe_prev = TAILQ_NEXT((elm), field);              \
        TRASHIT((elm)->field.tqe_next);                                 \
        TRASHIT((elm)->field.tqe_prev);                                 \
        QMD_TRACE_ELEM(&(elm)->field);                                  \
} while (0)

#define TAILQ_REMOVE_HEAD(head, field) do { \
        TAILQ_REMOVE(head, TAILQ_FIRST(head), field); \
        } while (0)

#define TAILQ_FIND(list, item, type, next, found) \
        do { \
        type curr_tf; \
        type next_tf; \
        \
        curr_tf = TAILQ_FIRST((list)); \
        while (curr_tf) \
        { \
            next_tf = TAILQ_NEXT(curr_tf, next); \
            if (curr_tf == (item))  \
            { \
                found = 1; \
                break; \
            } \
            curr_tf = next_tf; \
        } \
        } while(0); 
#define TAILQ_FIND_AND_REMOVE(list, item, type, next, removed) \
        do { \
        type curr_tf; \
        type next_tf; \
        \
        curr_tf = TAILQ_FIRST((list)); \
        while (curr_tf) \
        { \
            next_tf = TAILQ_NEXT(curr_tf, next); \
            if (curr_tf == (item))  \
            { \
                TAILQ_REMOVE((list), curr_tf, next); \
                removed = 1; \
                UNUSED_VAR(removed); \
            } \
            curr_tf = next_tf; \
        } \
        } while(0); 
#define TAILQ_INSERT_TAIL(head, elm, field) do {                        \
        TAILQ_NEXT((elm), field) = NULL;                                \
        (elm)->field.tqe_prev = (head)->tqh_last;                       \
        *(head)->tqh_last = (elm);                                      \
        (head)->tqh_last = &TAILQ_NEXT((elm), field);                   \
        QMD_TRACE_HEAD(head);                                           \
        QMD_TRACE_ELEM(&(elm)->field);                                  \
} while (0)
#define TAILQ_HEAD_INITIALIZER(head)                                    \
        { NULL, &(head).tqh_first }
#define TAILQ_INIT(head) do {                                           \
        TAILQ_FIRST((head)) = NULL;                                     \
        (head)->tqh_last = &TAILQ_FIRST((head));                        \
        QMD_TRACE_HEAD(head);                                           \
} while (0)
#define TAILQ_INSERT_BEFORE(listelm, elm, field) do {                   \
        (elm)->field.tqe_prev = (listelm)->field.tqe_prev;              \
        TAILQ_NEXT((elm), field) = (listelm);                           \
        *(listelm)->field.tqe_prev = (elm);                             \
        (listelm)->field.tqe_prev = &TAILQ_NEXT((elm), field);          \
        QMD_TRACE_ELEM(&(elm)->field);                                  \
        QMD_TRACE_ELEM(&listelm->field);                                \
} while (0)
#define TAILQ_LAST(head, headname)                                      \
        (*(((struct headname *)((head)->tqh_last))->tqh_last))
#define TAILQ_FOREACH(var, head, field)                                 \
        for ((var) = TAILQ_FIRST((head));                               \
            (var);                                                      \
            (var) = TAILQ_NEXT((var), field))

/*
 * Singly-linked Tail queue declarations.
 */
#define STAILQ_HEAD(name, type)                                         \
struct name {                                                           \
        struct type *stqh_first;/* first element */                     \
        struct type **stqh_last;/* addr of last next element */         \
}
#define STAILQ_ENTRY(type)                                              \
struct {                                                                \
        struct type *stqe_next; /* next element */                      \
}
#define STAILQ_FIRST(head)      ((head)->stqh_first)
#define SIMPLEQ_FIRST           STAILQ_FIRST
#define STAILQ_NEXT(elm, field) ((elm)->field.stqe_next)
#define STAILQ_REMOVE_HEAD(head, field) do {                            \
        if ((STAILQ_FIRST((head)) =                                     \
             STAILQ_NEXT(STAILQ_FIRST((head)), field)) == NULL)         \
                (head)->stqh_last = &STAILQ_FIRST((head));              \
} while (0)
#define STAILQ_INSERT_HEAD(head, elm, field) do {                       \
        if ((STAILQ_NEXT((elm), field) = STAILQ_FIRST((head))) == NULL) \
                (head)->stqh_last = &STAILQ_NEXT((elm), field);         \
        STAILQ_FIRST((head)) = (elm);                                   \
} while (0)
#define STAILQ_INSERT_TAIL(head, elm, field) do {                       \
        STAILQ_NEXT((elm), field) = NULL;                               \
        *(head)->stqh_last = (elm);                                     \
        (head)->stqh_last = &STAILQ_NEXT((elm), field);                 \
} while (0)
#define STAILQ_FOREACH(var, head, field)                                \
        for((var) = STAILQ_FIRST((head));                               \
           (var);                                                       \
           (var) = STAILQ_NEXT((var), field))
#define STAILQ_EMPTY(head)      ((head)->stqh_first == NULL)
#define LIST_INIT(head) do {                                            \
        LIST_FIRST((head)) = NULL;                                      \
} while (0)
#define LIST_REMOVE(elm, field) do {                                    \
        if (LIST_NEXT((elm), field) != NULL)                            \
                LIST_NEXT((elm), field)->field.le_prev =                \
                    (elm)->field.le_prev;                               \
        *(elm)->field.le_prev = LIST_NEXT((elm), field);                \
} while (0)
#define LIST_INSERT_HEAD(head, elm, field) do {                         \
        if ((LIST_NEXT((elm), field) = LIST_FIRST((head))) != NULL)     \
                LIST_FIRST((head))->field.le_prev = &LIST_NEXT((elm), field);\
        LIST_FIRST((head)) = (elm);                                     \
        (elm)->field.le_prev = &LIST_FIRST((head));                     \
} while (0)
#define STAILQ_INIT(head) do {                                          \
        STAILQ_FIRST((head)) = NULL;                                    \
        (head)->stqh_last = &STAILQ_FIRST((head));                      \
} while (0)
#define SIMPLEQ_REMOVE_HEAD     STAILQ_REMOVE_HEAD
#define SIMPLEQ_INSERT_HEAD     STAILQ_INSERT_HEAD
#define SIMPLEQ_INSERT_TAIL     STAILQ_INSERT_TAIL

#ifdef LIST_HEAD
#undef LIST_HEAD
#endif
#define LIST_HEAD(name, type)                                           \
struct name {                                                           \
        struct type *lh_first;  /* first element */                     \
}
#define LIST_ENTRY(type)                                                \
struct {                                                                \
        struct type *le_next;   /* next element */                      \
        struct type **le_prev;  /* address of previous next element */  \
}
#define LIST_EMPTY(head)        ((head)->lh_first == NULL)
#define LIST_FIRST(head)        ((head)->lh_first)
#define LIST_NEXT(elm, field)   ((elm)->field.le_next)
#define LIST_HEAD_INITIALIZER(head)  { NULL }
#define LIST_FOREACH(var, head, field)                                  \
        for ((var) = LIST_FIRST((head));                                \
            (var);                                                      \
            (var) = LIST_NEXT((var), field))
#define LIST_FOREACH_SAFE(var, head, field, tvar)                       \
        for ((var) = LIST_FIRST((head));                                \
            (var) && ((tvar) = LIST_NEXT((var), field), 1);             \
            (var) = (tvar))
                             
#endif
#endif

