; ModuleID = 'reomp_data_race.cpp'
source_filename = "reomp_data_race.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@stderr = external global ptr, align 8
@.str = private unnamed_addr constant [45 x i8] c"Test: 6666data_race: time = %f (ret:  %15d)\0A\00", align 1, !dbg !0

; Function Attrs: mustprogress noinline norecurse optnone uwtable
define dso_local noundef i32 @main(i32 noundef %0, ptr noundef %1) #0 !dbg !262 {
  call void @reomp_control(i32 0, i64 100, i32 1)
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca ptr, align 8
  %6 = alloca i32, align 4
  %7 = alloca double, align 8
  %8 = alloca double, align 8
  %9 = alloca double, align 8
  %10 = alloca double, align 8
  %11 = alloca i32, align 4
  store i32 0, ptr %3, align 4
  store i32 %0, ptr %4, align 4
  call void @llvm.dbg.declare(metadata ptr %4, metadata !266, metadata !DIExpression()), !dbg !267
  store ptr %1, ptr %5, align 8
  call void @llvm.dbg.declare(metadata ptr %5, metadata !268, metadata !DIExpression()), !dbg !269
  call void @llvm.dbg.declare(metadata ptr %6, metadata !270, metadata !DIExpression()), !dbg !271
  call void @llvm.dbg.declare(metadata ptr %7, metadata !272, metadata !DIExpression()), !dbg !273
  call void @llvm.dbg.declare(metadata ptr %8, metadata !274, metadata !DIExpression()), !dbg !275
  call void @llvm.dbg.declare(metadata ptr %9, metadata !276, metadata !DIExpression()), !dbg !277
  call void @llvm.dbg.declare(metadata ptr %10, metadata !278, metadata !DIExpression()), !dbg !279
  %12 = call i32 @MPI_Init(ptr noundef %4, ptr noundef %5), !dbg !280
  call void @llvm.dbg.declare(metadata ptr %11, metadata !281, metadata !DIExpression()), !dbg !282
  %13 = call noundef i32 @_ZL23reomp_example_data_racei(i32 noundef 2), !dbg !283
  store i32 %13, ptr %11, align 4, !dbg !282
  %14 = load ptr, ptr @stderr, align 8, !dbg !284
  %15 = load double, ptr %10, align 8, !dbg !285
  %16 = load double, ptr %9, align 8, !dbg !286
  %17 = fsub double %15, %16, !dbg !287
  %18 = load i32, ptr %11, align 4, !dbg !288
  %19 = call i32 (ptr, ptr, ...) @fprintf(ptr noundef %14, ptr noundef @.str, double noundef %17, i32 noundef %18), !dbg !289
  call void @reomp_control(i32 1, i64 100, i32 1), !dbg !290
  ret i32 0, !dbg !290
}

; Function Attrs: nocallback nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare i32 @MPI_Init(ptr noundef, ptr noundef) #2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
define internal noundef i32 @_ZL23reomp_example_data_racei(i32 noundef %0) #3 !dbg !291 {
  %2 = alloca i32, align 4
  %3 = alloca i64, align 8
  %4 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  call void @llvm.dbg.declare(metadata ptr %2, metadata !292, metadata !DIExpression()), !dbg !293
  call void @llvm.dbg.declare(metadata ptr %3, metadata !294, metadata !DIExpression()), !dbg !299
  call void @llvm.dbg.declare(metadata ptr %4, metadata !300, metadata !DIExpression()), !dbg !302
  store volatile i32 1, ptr %4, align 4, !dbg !302
  store i64 0, ptr %3, align 8, !dbg !303
  br label %5, !dbg !305

5:                                                ; preds = %15, %1
  %6 = load i64, ptr %3, align 8, !dbg !306
  %7 = load i32, ptr %2, align 4, !dbg !308
  %8 = sext i32 %7 to i64, !dbg !308
  %9 = sdiv i64 8, %8, !dbg !309
  %10 = icmp ult i64 %6, %9, !dbg !310
  br i1 %10, label %11, label %18, !dbg !311

11:                                               ; preds = %5
  %12 = load i32, ptr %2, align 4, !dbg !312
  call void @reomp_control(i32 10, i64 101, i32 2), !dbg !314
  %13 = load volatile i32, ptr %4, align 4, !dbg !314
  call void @reomp_control(i32 11, i64 101, i32 2), !dbg !314
  %14 = add nsw i32 %13, %12, !dbg !314
  call void @reomp_control(i32 10, i64 102, i32 2), !dbg !314
  store volatile i32 %14, ptr %4, align 4, !dbg !314
  call void @reomp_control(i32 11, i64 102, i32 2), !dbg !315
  br label %15, !dbg !315

15:                                               ; preds = %11
  %16 = load i64, ptr %3, align 8, !dbg !316
  %17 = add i64 %16, 1, !dbg !316
  store i64 %17, ptr %3, align 8, !dbg !316
  br label %5, !dbg !317, !llvm.loop !318

18:                                               ; preds = %5
  %19 = load volatile i32, ptr %4, align 4, !dbg !321
  ret i32 %19, !dbg !322
}

declare i32 @fprintf(ptr noundef, ptr noundef, ...) #2

declare void @reomp_control(i32, i64, i32)

attributes #0 = { mustprogress noinline norecurse optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nocallback nofree nosync nounwind readnone speculatable willreturn }
attributes #2 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { mustprogress noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.dbg.cu = !{!8}
!llvm.module.flags = !{!254, !255, !256, !257, !258, !259, !260}
!llvm.ident = !{!261}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(scope: null, file: !2, line: 35, type: !3, isLocal: true, isDefinition: true)
!2 = !DIFile(filename: "reomp_data_race.cpp", directory: "/home/msm924/rempipro/ReMPI/test/reomp", checksumkind: CSK_MD5, checksum: "9a5a77502a4b52304933350006bb0176")
!3 = !DICompositeType(tag: DW_TAG_array_type, baseType: !4, size: 360, elements: !6)
!4 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !5)
!5 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!6 = !{!7}
!7 = !DISubrange(count: 45)
!8 = distinct !DICompileUnit(language: DW_LANG_C_plus_plus_14, file: !2, producer: "clang version 15.0.0", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, globals: !9, imports: !10, splitDebugInlining: false, nameTableKind: None)
!9 = !{!0}
!10 = !{!11, !19, !23, !30, !34, !39, !41, !47, !51, !55, !69, !73, !77, !81, !85, !90, !94, !98, !102, !106, !114, !118, !122, !124, !128, !132, !137, !143, !147, !151, !153, !161, !165, !173, !175, !179, !183, !187, !191, !196, !201, !206, !207, !208, !209, !211, !212, !213, !214, !215, !216, !217, !219, !220, !221, !222, !223, !224, !225, !230, !231, !232, !233, !234, !235, !236, !237, !238, !239, !240, !241, !242, !243, !244, !245, !246, !247, !248, !249, !250, !251, !252, !253}
!11 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !13, file: !18, line: 52)
!12 = !DINamespace(name: "std", scope: null)
!13 = !DISubprogram(name: "abs", scope: !14, file: !14, line: 837, type: !15, flags: DIFlagPrototyped, spFlags: 0)
!14 = !DIFile(filename: "/usr/include/stdlib.h", directory: "", checksumkind: CSK_MD5, checksum: "d0b67d4c866748c04ac2b355c26c1c70")
!15 = !DISubroutineType(types: !16)
!16 = !{!17, !17}
!17 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!18 = !DIFile(filename: "/usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/bits/std_abs.h", directory: "")
!19 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !20, file: !22, line: 127)
!20 = !DIDerivedType(tag: DW_TAG_typedef, name: "div_t", file: !14, line: 62, baseType: !21)
!21 = !DICompositeType(tag: DW_TAG_structure_type, file: !14, line: 58, size: 64, flags: DIFlagFwdDecl, identifier: "_ZTS5div_t")
!22 = !DIFile(filename: "/usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/cstdlib", directory: "")
!23 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !24, file: !22, line: 128)
!24 = !DIDerivedType(tag: DW_TAG_typedef, name: "ldiv_t", file: !14, line: 70, baseType: !25)
!25 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !14, line: 66, size: 128, flags: DIFlagTypePassByValue, elements: !26, identifier: "_ZTS6ldiv_t")
!26 = !{!27, !29}
!27 = !DIDerivedType(tag: DW_TAG_member, name: "quot", scope: !25, file: !14, line: 68, baseType: !28, size: 64)
!28 = !DIBasicType(name: "long", size: 64, encoding: DW_ATE_signed)
!29 = !DIDerivedType(tag: DW_TAG_member, name: "rem", scope: !25, file: !14, line: 69, baseType: !28, size: 64, offset: 64)
!30 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !31, file: !22, line: 130)
!31 = !DISubprogram(name: "abort", scope: !14, file: !14, line: 588, type: !32, flags: DIFlagPrototyped | DIFlagNoReturn, spFlags: 0)
!32 = !DISubroutineType(types: !33)
!33 = !{null}
!34 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !35, file: !22, line: 134)
!35 = !DISubprogram(name: "atexit", scope: !14, file: !14, line: 592, type: !36, flags: DIFlagPrototyped, spFlags: 0)
!36 = !DISubroutineType(types: !37)
!37 = !{!17, !38}
!38 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !32, size: 64)
!39 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !40, file: !22, line: 137)
!40 = !DISubprogram(name: "at_quick_exit", scope: !14, file: !14, line: 597, type: !36, flags: DIFlagPrototyped, spFlags: 0)
!41 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !42, file: !22, line: 140)
!42 = !DISubprogram(name: "atof", scope: !14, file: !14, line: 101, type: !43, flags: DIFlagPrototyped, spFlags: 0)
!43 = !DISubroutineType(types: !44)
!44 = !{!45, !46}
!45 = !DIBasicType(name: "double", size: 64, encoding: DW_ATE_float)
!46 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !4, size: 64)
!47 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !48, file: !22, line: 141)
!48 = !DISubprogram(name: "atoi", scope: !14, file: !14, line: 104, type: !49, flags: DIFlagPrototyped, spFlags: 0)
!49 = !DISubroutineType(types: !50)
!50 = !{!17, !46}
!51 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !52, file: !22, line: 142)
!52 = !DISubprogram(name: "atol", scope: !14, file: !14, line: 107, type: !53, flags: DIFlagPrototyped, spFlags: 0)
!53 = !DISubroutineType(types: !54)
!54 = !{!28, !46}
!55 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !56, file: !22, line: 143)
!56 = !DISubprogram(name: "bsearch", scope: !14, file: !14, line: 817, type: !57, flags: DIFlagPrototyped, spFlags: 0)
!57 = !DISubroutineType(types: !58)
!58 = !{!59, !60, !60, !62, !62, !65}
!59 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: 64)
!60 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !61, size: 64)
!61 = !DIDerivedType(tag: DW_TAG_const_type, baseType: null)
!62 = !DIDerivedType(tag: DW_TAG_typedef, name: "size_t", file: !63, line: 46, baseType: !64)
!63 = !DIFile(filename: "/lustre/home/msm924/llvm15/lib/clang/15.0.0/include/stddef.h", directory: "", checksumkind: CSK_MD5, checksum: "b76978376d35d5cd171876ac58ac1256")
!64 = !DIBasicType(name: "unsigned long", size: 64, encoding: DW_ATE_unsigned)
!65 = !DIDerivedType(tag: DW_TAG_typedef, name: "__compar_fn_t", file: !14, line: 805, baseType: !66)
!66 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !67, size: 64)
!67 = !DISubroutineType(types: !68)
!68 = !{!17, !60, !60}
!69 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !70, file: !22, line: 144)
!70 = !DISubprogram(name: "calloc", scope: !14, file: !14, line: 541, type: !71, flags: DIFlagPrototyped, spFlags: 0)
!71 = !DISubroutineType(types: !72)
!72 = !{!59, !62, !62}
!73 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !74, file: !22, line: 145)
!74 = !DISubprogram(name: "div", scope: !14, file: !14, line: 849, type: !75, flags: DIFlagPrototyped, spFlags: 0)
!75 = !DISubroutineType(types: !76)
!76 = !{!20, !17, !17}
!77 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !78, file: !22, line: 146)
!78 = !DISubprogram(name: "exit", scope: !14, file: !14, line: 614, type: !79, flags: DIFlagPrototyped | DIFlagNoReturn, spFlags: 0)
!79 = !DISubroutineType(types: !80)
!80 = !{null, !17}
!81 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !82, file: !22, line: 147)
!82 = !DISubprogram(name: "free", scope: !14, file: !14, line: 563, type: !83, flags: DIFlagPrototyped, spFlags: 0)
!83 = !DISubroutineType(types: !84)
!84 = !{null, !59}
!85 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !86, file: !22, line: 148)
!86 = !DISubprogram(name: "getenv", scope: !14, file: !14, line: 631, type: !87, flags: DIFlagPrototyped, spFlags: 0)
!87 = !DISubroutineType(types: !88)
!88 = !{!89, !46}
!89 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !5, size: 64)
!90 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !91, file: !22, line: 149)
!91 = !DISubprogram(name: "labs", scope: !14, file: !14, line: 838, type: !92, flags: DIFlagPrototyped, spFlags: 0)
!92 = !DISubroutineType(types: !93)
!93 = !{!28, !28}
!94 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !95, file: !22, line: 150)
!95 = !DISubprogram(name: "ldiv", scope: !14, file: !14, line: 851, type: !96, flags: DIFlagPrototyped, spFlags: 0)
!96 = !DISubroutineType(types: !97)
!97 = !{!24, !28, !28}
!98 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !99, file: !22, line: 151)
!99 = !DISubprogram(name: "malloc", scope: !14, file: !14, line: 539, type: !100, flags: DIFlagPrototyped, spFlags: 0)
!100 = !DISubroutineType(types: !101)
!101 = !{!59, !62}
!102 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !103, file: !22, line: 153)
!103 = !DISubprogram(name: "mblen", scope: !14, file: !14, line: 919, type: !104, flags: DIFlagPrototyped, spFlags: 0)
!104 = !DISubroutineType(types: !105)
!105 = !{!17, !46, !62}
!106 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !107, file: !22, line: 154)
!107 = !DISubprogram(name: "mbstowcs", scope: !14, file: !14, line: 930, type: !108, flags: DIFlagPrototyped, spFlags: 0)
!108 = !DISubroutineType(types: !109)
!109 = !{!62, !110, !113, !62}
!110 = !DIDerivedType(tag: DW_TAG_restrict_type, baseType: !111)
!111 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !112, size: 64)
!112 = !DIBasicType(name: "wchar_t", size: 32, encoding: DW_ATE_signed)
!113 = !DIDerivedType(tag: DW_TAG_restrict_type, baseType: !46)
!114 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !115, file: !22, line: 155)
!115 = !DISubprogram(name: "mbtowc", scope: !14, file: !14, line: 922, type: !116, flags: DIFlagPrototyped, spFlags: 0)
!116 = !DISubroutineType(types: !117)
!117 = !{!17, !110, !113, !62}
!118 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !119, file: !22, line: 157)
!119 = !DISubprogram(name: "qsort", scope: !14, file: !14, line: 827, type: !120, flags: DIFlagPrototyped, spFlags: 0)
!120 = !DISubroutineType(types: !121)
!121 = !{null, !59, !62, !62, !65}
!122 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !123, file: !22, line: 160)
!123 = !DISubprogram(name: "quick_exit", scope: !14, file: !14, line: 620, type: !79, flags: DIFlagPrototyped | DIFlagNoReturn, spFlags: 0)
!124 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !125, file: !22, line: 163)
!125 = !DISubprogram(name: "rand", scope: !14, file: !14, line: 453, type: !126, flags: DIFlagPrototyped, spFlags: 0)
!126 = !DISubroutineType(types: !127)
!127 = !{!17}
!128 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !129, file: !22, line: 164)
!129 = !DISubprogram(name: "realloc", scope: !14, file: !14, line: 549, type: !130, flags: DIFlagPrototyped, spFlags: 0)
!130 = !DISubroutineType(types: !131)
!131 = !{!59, !59, !62}
!132 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !133, file: !22, line: 165)
!133 = !DISubprogram(name: "srand", scope: !14, file: !14, line: 455, type: !134, flags: DIFlagPrototyped, spFlags: 0)
!134 = !DISubroutineType(types: !135)
!135 = !{null, !136}
!136 = !DIBasicType(name: "unsigned int", size: 32, encoding: DW_ATE_unsigned)
!137 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !138, file: !22, line: 166)
!138 = !DISubprogram(name: "strtod", scope: !14, file: !14, line: 117, type: !139, flags: DIFlagPrototyped, spFlags: 0)
!139 = !DISubroutineType(types: !140)
!140 = !{!45, !113, !141}
!141 = !DIDerivedType(tag: DW_TAG_restrict_type, baseType: !142)
!142 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !89, size: 64)
!143 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !144, file: !22, line: 167)
!144 = !DISubprogram(name: "strtol", scope: !14, file: !14, line: 176, type: !145, flags: DIFlagPrototyped, spFlags: 0)
!145 = !DISubroutineType(types: !146)
!146 = !{!28, !113, !141, !17}
!147 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !148, file: !22, line: 168)
!148 = !DISubprogram(name: "strtoul", scope: !14, file: !14, line: 180, type: !149, flags: DIFlagPrototyped, spFlags: 0)
!149 = !DISubroutineType(types: !150)
!150 = !{!64, !113, !141, !17}
!151 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !152, file: !22, line: 169)
!152 = !DISubprogram(name: "system", scope: !14, file: !14, line: 781, type: !49, flags: DIFlagPrototyped, spFlags: 0)
!153 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !154, file: !22, line: 171)
!154 = !DISubprogram(name: "wcstombs", scope: !14, file: !14, line: 933, type: !155, flags: DIFlagPrototyped, spFlags: 0)
!155 = !DISubroutineType(types: !156)
!156 = !{!62, !157, !158, !62}
!157 = !DIDerivedType(tag: DW_TAG_restrict_type, baseType: !89)
!158 = !DIDerivedType(tag: DW_TAG_restrict_type, baseType: !159)
!159 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !160, size: 64)
!160 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !112)
!161 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !162, file: !22, line: 172)
!162 = !DISubprogram(name: "wctomb", scope: !14, file: !14, line: 926, type: !163, flags: DIFlagPrototyped, spFlags: 0)
!163 = !DISubroutineType(types: !164)
!164 = !{!17, !89, !112}
!165 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !166, entity: !167, file: !22, line: 200)
!166 = !DINamespace(name: "__gnu_cxx", scope: null)
!167 = !DIDerivedType(tag: DW_TAG_typedef, name: "lldiv_t", file: !14, line: 80, baseType: !168)
!168 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !14, line: 76, size: 128, flags: DIFlagTypePassByValue, elements: !169, identifier: "_ZTS7lldiv_t")
!169 = !{!170, !172}
!170 = !DIDerivedType(tag: DW_TAG_member, name: "quot", scope: !168, file: !14, line: 78, baseType: !171, size: 64)
!171 = !DIBasicType(name: "long long", size: 64, encoding: DW_ATE_signed)
!172 = !DIDerivedType(tag: DW_TAG_member, name: "rem", scope: !168, file: !14, line: 79, baseType: !171, size: 64, offset: 64)
!173 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !166, entity: !174, file: !22, line: 206)
!174 = !DISubprogram(name: "_Exit", scope: !14, file: !14, line: 626, type: !79, flags: DIFlagPrototyped | DIFlagNoReturn, spFlags: 0)
!175 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !166, entity: !176, file: !22, line: 210)
!176 = !DISubprogram(name: "llabs", scope: !14, file: !14, line: 841, type: !177, flags: DIFlagPrototyped, spFlags: 0)
!177 = !DISubroutineType(types: !178)
!178 = !{!171, !171}
!179 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !166, entity: !180, file: !22, line: 216)
!180 = !DISubprogram(name: "lldiv", scope: !14, file: !14, line: 855, type: !181, flags: DIFlagPrototyped, spFlags: 0)
!181 = !DISubroutineType(types: !182)
!182 = !{!167, !171, !171}
!183 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !166, entity: !184, file: !22, line: 227)
!184 = !DISubprogram(name: "atoll", scope: !14, file: !14, line: 112, type: !185, flags: DIFlagPrototyped, spFlags: 0)
!185 = !DISubroutineType(types: !186)
!186 = !{!171, !46}
!187 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !166, entity: !188, file: !22, line: 228)
!188 = !DISubprogram(name: "strtoll", scope: !14, file: !14, line: 200, type: !189, flags: DIFlagPrototyped, spFlags: 0)
!189 = !DISubroutineType(types: !190)
!190 = !{!171, !113, !141, !17}
!191 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !166, entity: !192, file: !22, line: 229)
!192 = !DISubprogram(name: "strtoull", scope: !14, file: !14, line: 205, type: !193, flags: DIFlagPrototyped, spFlags: 0)
!193 = !DISubroutineType(types: !194)
!194 = !{!195, !113, !141, !17}
!195 = !DIBasicType(name: "unsigned long long", size: 64, encoding: DW_ATE_unsigned)
!196 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !166, entity: !197, file: !22, line: 231)
!197 = !DISubprogram(name: "strtof", scope: !14, file: !14, line: 123, type: !198, flags: DIFlagPrototyped, spFlags: 0)
!198 = !DISubroutineType(types: !199)
!199 = !{!200, !113, !141}
!200 = !DIBasicType(name: "float", size: 32, encoding: DW_ATE_float)
!201 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !166, entity: !202, file: !22, line: 232)
!202 = !DISubprogram(name: "strtold", scope: !14, file: !14, line: 126, type: !203, flags: DIFlagPrototyped, spFlags: 0)
!203 = !DISubroutineType(types: !204)
!204 = !{!205, !113, !141}
!205 = !DIBasicType(name: "long double", size: 128, encoding: DW_ATE_float)
!206 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !167, file: !22, line: 240)
!207 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !174, file: !22, line: 242)
!208 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !176, file: !22, line: 244)
!209 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !210, file: !22, line: 245)
!210 = !DISubprogram(name: "div", linkageName: "_ZN9__gnu_cxx3divExx", scope: !166, file: !22, line: 213, type: !181, flags: DIFlagPrototyped, spFlags: 0)
!211 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !180, file: !22, line: 246)
!212 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !184, file: !22, line: 248)
!213 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !197, file: !22, line: 249)
!214 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !188, file: !22, line: 250)
!215 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !192, file: !22, line: 251)
!216 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !12, entity: !202, file: !22, line: 252)
!217 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !31, file: !218, line: 38)
!218 = !DIFile(filename: "/usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/stdlib.h", directory: "", checksumkind: CSK_MD5, checksum: "ceef6bc8df57c14109c48fd732ade0bb")
!219 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !35, file: !218, line: 39)
!220 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !78, file: !218, line: 40)
!221 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !40, file: !218, line: 43)
!222 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !123, file: !218, line: 46)
!223 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !20, file: !218, line: 51)
!224 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !24, file: !218, line: 52)
!225 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !226, file: !218, line: 54)
!226 = !DISubprogram(name: "abs", linkageName: "_ZSt3absg", scope: !12, file: !18, line: 102, type: !227, flags: DIFlagPrototyped, spFlags: 0)
!227 = !DISubroutineType(types: !228)
!228 = !{!229, !229}
!229 = !DIBasicType(name: "__float128", size: 128, encoding: DW_ATE_float)
!230 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !42, file: !218, line: 55)
!231 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !48, file: !218, line: 56)
!232 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !52, file: !218, line: 57)
!233 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !56, file: !218, line: 58)
!234 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !70, file: !218, line: 59)
!235 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !210, file: !218, line: 60)
!236 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !82, file: !218, line: 61)
!237 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !86, file: !218, line: 62)
!238 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !91, file: !218, line: 63)
!239 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !95, file: !218, line: 64)
!240 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !99, file: !218, line: 65)
!241 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !103, file: !218, line: 67)
!242 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !107, file: !218, line: 68)
!243 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !115, file: !218, line: 69)
!244 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !119, file: !218, line: 71)
!245 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !125, file: !218, line: 72)
!246 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !129, file: !218, line: 73)
!247 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !133, file: !218, line: 74)
!248 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !138, file: !218, line: 75)
!249 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !144, file: !218, line: 76)
!250 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !148, file: !218, line: 77)
!251 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !152, file: !218, line: 78)
!252 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !154, file: !218, line: 80)
!253 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !8, entity: !162, file: !218, line: 81)
!254 = !{i32 7, !"Dwarf Version", i32 5}
!255 = !{i32 2, !"Debug Info Version", i32 3}
!256 = !{i32 1, !"wchar_size", i32 4}
!257 = !{i32 7, !"PIC Level", i32 2}
!258 = !{i32 7, !"PIE Level", i32 2}
!259 = !{i32 7, !"uwtable", i32 2}
!260 = !{i32 7, !"frame-pointer", i32 2}
!261 = !{!"clang version 15.0.0"}
!262 = distinct !DISubprogram(name: "main", scope: !2, file: !2, line: 19, type: !263, scopeLine: 20, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !8, retainedNodes: !265)
!263 = !DISubroutineType(types: !264)
!264 = !{!17, !17, !142}
!265 = !{}
!266 = !DILocalVariable(name: "argc", arg: 1, scope: !262, file: !2, line: 19, type: !17)
!267 = !DILocation(line: 19, column: 14, scope: !262)
!268 = !DILocalVariable(name: "argv", arg: 2, scope: !262, file: !2, line: 19, type: !142)
!269 = !DILocation(line: 19, column: 27, scope: !262)
!270 = !DILocalVariable(name: "nth", scope: !262, file: !2, line: 21, type: !17)
!271 = !DILocation(line: 21, column: 9, scope: !262)
!272 = !DILocalVariable(name: "whole_s", scope: !262, file: !2, line: 22, type: !45)
!273 = !DILocation(line: 22, column: 12, scope: !262)
!274 = !DILocalVariable(name: "whole_e", scope: !262, file: !2, line: 22, type: !45)
!275 = !DILocation(line: 22, column: 21, scope: !262)
!276 = !DILocalVariable(name: "s", scope: !262, file: !2, line: 22, type: !45)
!277 = !DILocation(line: 22, column: 30, scope: !262)
!278 = !DILocalVariable(name: "e", scope: !262, file: !2, line: 22, type: !45)
!279 = !DILocation(line: 22, column: 33, scope: !262)
!280 = !DILocation(line: 24, column: 5, scope: !262)
!281 = !DILocalVariable(name: "ret1", scope: !262, file: !2, line: 33, type: !17)
!282 = !DILocation(line: 33, column: 9, scope: !262)
!283 = !DILocation(line: 33, column: 16, scope: !262)
!284 = !DILocation(line: 35, column: 13, scope: !262)
!285 = !DILocation(line: 35, column: 70, scope: !262)
!286 = !DILocation(line: 35, column: 74, scope: !262)
!287 = !DILocation(line: 35, column: 72, scope: !262)
!288 = !DILocation(line: 35, column: 77, scope: !262)
!289 = !DILocation(line: 35, column: 5, scope: !262)
!290 = !DILocation(line: 38, column: 5, scope: !262)
!291 = distinct !DISubprogram(name: "reomp_example_data_race", linkageName: "_ZL23reomp_example_data_racei", scope: !2, file: !2, line: 8, type: !15, scopeLine: 9, flags: DIFlagPrototyped, spFlags: DISPFlagLocalToUnit | DISPFlagDefinition, unit: !8, retainedNodes: !265)
!292 = !DILocalVariable(name: "nth", arg: 1, scope: !291, file: !2, line: 8, type: !17)
!293 = !DILocation(line: 8, column: 40, scope: !291)
!294 = !DILocalVariable(name: "i", scope: !291, file: !2, line: 10, type: !295)
!295 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint64_t", file: !296, line: 27, baseType: !297)
!296 = !DIFile(filename: "/usr/include/bits/stdint-uintn.h", directory: "", checksumkind: CSK_MD5, checksum: "9754ebe022edbe8d7928fa709e442f0d")
!297 = !DIDerivedType(tag: DW_TAG_typedef, name: "__uint64_t", file: !298, line: 44, baseType: !64)
!298 = !DIFile(filename: "/usr/include/bits/types.h", directory: "", checksumkind: CSK_MD5, checksum: "eac2c46b20ddc2be81186b6ffebfd845")
!299 = !DILocation(line: 10, column: 14, scope: !291)
!300 = !DILocalVariable(name: "sum", scope: !291, file: !2, line: 11, type: !301)
!301 = !DIDerivedType(tag: DW_TAG_volatile_type, baseType: !17)
!302 = !DILocation(line: 11, column: 18, scope: !291)
!303 = !DILocation(line: 13, column: 12, scope: !304)
!304 = distinct !DILexicalBlock(scope: !291, file: !2, line: 13, column: 5)
!305 = !DILocation(line: 13, column: 10, scope: !304)
!306 = !DILocation(line: 13, column: 17, scope: !307)
!307 = distinct !DILexicalBlock(scope: !304, file: !2, line: 13, column: 5)
!308 = !DILocation(line: 13, column: 26, scope: !307)
!309 = !DILocation(line: 13, column: 24, scope: !307)
!310 = !DILocation(line: 13, column: 19, scope: !307)
!311 = !DILocation(line: 13, column: 5, scope: !304)
!312 = !DILocation(line: 14, column: 16, scope: !313)
!313 = distinct !DILexicalBlock(scope: !307, file: !2, line: 13, column: 36)
!314 = !DILocation(line: 14, column: 13, scope: !313)
!315 = !DILocation(line: 15, column: 5, scope: !313)
!316 = !DILocation(line: 13, column: 32, scope: !307)
!317 = !DILocation(line: 13, column: 5, scope: !307)
!318 = distinct !{!318, !311, !319, !320}
!319 = !DILocation(line: 15, column: 5, scope: !304)
!320 = !{!"llvm.loop.mustprogress"}
!321 = !DILocation(line: 16, column: 12, scope: !291)
!322 = !DILocation(line: 16, column: 5, scope: !291)
