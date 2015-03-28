; ModuleID = 'basic.ll'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nounwind uwtable
define i32 @sum(i32 %a, i32 %b) #0 {
bb:
  br label %bb1

bb1:                                              ; preds = %bb4, %bb
  %i.0 = phi i32 [ %a, %bb ], [ %tmp5, %bb4 ]
  %res.0 = phi i32 [ 1, %bb ], [ %tmp3, %bb4 ]
  %tmp = icmp slt i32 %i.0, %b
  br i1 %tmp, label %bb2, label %bb6

bb2:                                              ; preds = %bb1
  %tmp3 = mul nsw i32 %res.0, %i.0
  br label %bb4

bb4:                                              ; preds = %bb2
  %tmp5 = add nsw i32 %i.0, 1
  br label %bb1

bb6:                                              ; preds = %bb1
  ret i32 %res.0
}

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.6.0 (tags/RELEASE_360/final)"}
