; ModuleID = 'basic1.bc'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@sum.c = private unnamed_addr constant [5 x i32] [i32 1, i32 2, i32 3, i32 4, i32 5], align 16

; Function Attrs: nounwind uwtable
define i32 @temp(i32 %e) #0 {
entry:
  %mul = mul nsw i32 %e, 5
  ret i32 %mul
}

; Function Attrs: nounwind uwtable
define i32 @temp1(i32 %e) #0 {
entry:
  %add = add nsw i32 %e, 5
  ret i32 %add
}

; Function Attrs: nounwind uwtable
define void @temp2(i32 %f) #0 {
entry:
  %div = sdiv i32 %f, 2
  ret void
}

; Function Attrs: nounwind uwtable
define i32 @sum(i32 %a, i32 %b) #0 {
entry:
  %c = alloca [5 x i32], align 16
  %tmp = bitcast [5 x i32]* %c to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %tmp, i8* bitcast ([5 x i32]* @sum.c to i8*), i64 20, i32 16, i1 false)
  %cmp = icmp eq i32 %b, 4
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  br label %if.end

if.else:                                          ; preds = %entry
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %foo.0 = phi i32 (i32)* [ @temp, %if.then ], [ @temp1, %if.else ]
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %if.end
  %i.0 = phi i32 [ %a, %if.end ], [ %inc, %for.inc ]
  %cmp1 = icmp slt i32 %i.0, %b
  br i1 %cmp1, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %idxprom = sext i32 %i.0 to i64
  %arrayidx = getelementptr inbounds [5 x i32]* %c, i32 0, i64 %idxprom
  %tmp1 = load i32* %arrayidx, align 4
  %mul = mul nsw i32 %tmp1, %i.0
  store i32 %mul, i32* %arrayidx, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %inc = add nsw i32 %i.0, 1
  br label %for.cond

for.end:                                          ; preds = %for.cond
  %call = call i32 @temp(i32 %b)
  %call2 = call i32 %foo.0(i32 1)
  call void @temp2(i32 %call)
  %add = add nsw i32 %call2, %call
  %arrayidx3 = getelementptr inbounds [5 x i32]* %c, i32 0, i64 2
  store i32 %add, i32* %arrayidx3, align 4
  %arrayidx4 = getelementptr inbounds [5 x i32]* %c, i32 0, i64 2
  %tmp2 = load i32* %arrayidx4, align 4
  ret i32 %tmp2
}

; Function Attrs: nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture, i8* nocapture readonly, i64, i32, i1) #1

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"clang version 3.5.1 (tags/RELEASE_351/final)"}
